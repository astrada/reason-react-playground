/* Based on https://github.com/react-toolbox/react-toolbox/blob/dev/docs/app/components/preview/index.js */
let previewContainerStyle =
  ReactDOMRe.Style.make(
    ~display="flex",
    ~flexDirection="column",
    ~minHeight="300px",
    ~border="1px solid #aaa",
    ~position="relative",
    ~overflow="auto",
    ()
  );

let previewStyle = ReactDOMRe.Style.make(~padding="10px", ());

let contentStyle = ReactDOMRe.Style.make(~flexGrow="1", ~margin="10px", ());

type retainedProps = {code: string};

type state = {
  evalResult: Utils.evalResult,
  evaluatingJs: bool
};

type action =
  | UpdateEvalResult(Utils.evalResult);

let component = ReasonReact.reducerComponentWithRetainedProps("Preview");

let executeCode = (self: ReasonReact.self(state, retainedProps, action)) => {
  let code = self.ReasonReact.retainedProps.code;
  let evalResult = Utils.evalJs(code);
  self.send(UpdateEvalResult(evalResult));
};

let make = (~code, ~className=?, ~onDone: option(bool => unit)=?, _children) => {
  ...component,
  retainedProps: {
    code: code
  },
  initialState: () => {evalResult: Utils.Success, evaluatingJs: false},
  reducer: (action, state) =>
    switch (action) {
    | UpdateEvalResult(evalResult) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, evalResult, evaluatingJs: false},
        (
          self => {
            let error = self.state.evalResult != Utils.Success;
            switch (onDone) {
            | Some(onDone) => onDone(error)
            | None => ()
            };
          }
        )
      )
    },
  didMount: self => {
    executeCode(self);
    ReasonReact.NoUpdate;
  },
  didUpdate: ({oldSelf, newSelf}) =>
    if (oldSelf.retainedProps.code !== newSelf.retainedProps.code) {
      executeCode(newSelf);
    },
  render: (self: ReasonReact.self(state, retainedProps, action)) => {
    let className = Js.Option.getWithDefault("", className);
    let errorMessage =
      switch (self.state.evalResult) {
      | Utils.Success => None
      | Utils.ErrorMessage(e) => Some(e)
      };
    <div className style=previewContainerStyle>
      <div id="preview" style=previewStyle />
      <Error ?errorMessage />
    </div>;
  }
};
