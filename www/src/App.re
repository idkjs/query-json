open Belt;

type response = result(string, string);

[@bs.module "../../_build/default/js/Js.bc.js"]
external queryJson: (string, string) => response = "run";

let mockJson = {|{
  "store": {
    "books": [
      {
        "category": "reference",
        "author": "Nigel Rees",
        "title": "Sayings of the Century",
        "price": 8.95
      },
      {
        "category": "fiction",
        "author": "Evelyn Waugh",
        "title": "Sword of Honour",
        "price": 12.99
      },
      {
        "category": "fiction",
        "author": "Herman Melville",
        "title": "Moby Dick",
        "isbn": "0-553-21311-3",
        "price": 8.99
      },
      {
        "category": "fiction",
        "author": "J. R. R. Tolkien",
        "title": "The Lord of the Rings",
        "isbn": "0-395-19395-8",
        "price": 22.99
      }
    ]
  }
}
|};

module Menu = [%styled {|
  width: 75vw;
  height: 10vh;
|}];

module Header = {
  [@react.component]
  let make = () => {
    <Menu> <Text> "query-json playground" </Text> </Menu>;
  };
};

module SpacerBottom = [%styled "margin-bottom: 8px"];
module SpacerTop = [%styled "margin-top: 8px"];
module SpacerRight = [%styled "margin-right: 8px"];
module SpacerLeft = [%styled "margin-left: 8px; height: 100%;"];

module Page = [%styled
  {|
  display: flex;
  flex-direction: column;
  align-items: center;
  height: 100vh;
|}
];

module Container = [%styled.main
  {|
  width: 75vw;
  height: 80vh;

  display: flex;
  flex-direction: row;
  justify-content: space-around;
|}
];

module ColumnHalf = [%styled.div {|
  width: 50%;
  height: 100%;
|}];

module Stack = [%styled.div
  {|
  display: flex;
  flex-direction: column;

  height: 100%;
  |}
];

module Query = {
  [@react.component]
  let make = (~value, ~placeholder, ~onChange) => {
    let onChangeHandler = event => {
      let value = ReactEvent.Form.target(event)##value;
      onChange(value);
    };

    <input type_="text" value placeholder onChange=onChangeHandler />;
  };
};

module Json = {
  [@react.component]
  let make = (~value, ~onChange) => {
    let onChangeHandler = event => {
      let value = ReactEvent.Form.target(event)##value;
      onChange(value);
    };

    <textarea
      className=[%css "height: 100%;"]
      value
      onChange=onChangeHandler
    />;
  };
};

module Box = [%styled.div
  {|
  background: #C4C4C4;
  height: 100%;
  width: 100%;
|}
];

module EmptyResult = {
  [@react.component]
  let make = () => {
    <Box />;
  };
};

module Result = {
  [@react.component]
  let make = (~value: response) => {
    let text =
      switch (value) {
      | Ok(o) => o
      | Error(e) => e
      };

    <Box> <pre> <code> {React.string(text)} </code> </pre> </Box>;
  };
};

type t = {
  query: string,
  json: option(string),
  output: option(response),
};

type actions =
  | UpdateQuery(string)
  | UpdateJson(string)
  | ComputeOutput;

let reduce = (state, action) => {
  switch (action) {
  | UpdateQuery(query) => {...state, query}
  | UpdateJson(json) => {...state, json: Some(json)}
  | ComputeOutput =>
    switch (state.json) {
    | Some(json) =>
      let result = queryJson(state.query, json);
      {...state, output: Some(result)};
    | None => state
    }
  };
};

[@react.component]
let make = () => {
  let (state, dispatch) =
    React.useReducer(
      reduce,
      {query: "", json: Some(mockJson), output: None},
    );

  let onQueryChange = value => {
    dispatch(UpdateQuery(value));
    dispatch(ComputeOutput);
  };

  let onJsonChange = value => {
    dispatch(UpdateJson(value));
    dispatch(ComputeOutput);
  };

  <Page>
    <SpacerTop />
    <SpacerTop />
    <Header />
    <SpacerBottom />
    <Container>
      <ColumnHalf>
        <Stack>
          <Query
            value={state.query}
            placeholder="Type the filter, ex: '.'"
            onChange=onQueryChange
          />
          <SpacerBottom />
          <Json
            value={Option.getWithDefault(state.json, "")}
            onChange=onJsonChange
          />
        </Stack>
        <SpacerRight />
      </ColumnHalf>
      <ColumnHalf>
        <div className="non-scroll">
          <SpacerLeft>
            {switch (state.output) {
             | Some(value) => <Result value />
             | None => <EmptyResult />
             }}
          </SpacerLeft>
        </div>
      </ColumnHalf>
    </Container>
  </Page>;
};
