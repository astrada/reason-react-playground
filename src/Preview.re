/* Based on https://github.com/react-toolbox/react-toolbox/blob/dev/docs/app/components/preview/index.js */
let previewStyle =
  ReactDOMRe.Style.make
    display::"flex"
    flexDirection::"column"
    minHeight::"300px"
    border::"1px solid #aaa"
    position::"relative"
    overflow::"auto"
    margin::"20px 0 20px 0"
    ();

let contentStyle = ReactDOMRe.Style.make flexGrow::"1" margin::"10px" ();

type retainedProps = {code: string};

type state = {
  evalResult: Utils.evalResult,
  evaluatingJs: bool
};

type action =
  | UpdateEvalResult Utils.evalResult;

let component = ReasonReact.reducerComponentWithRetainedProps "Preview";

let executeCode (self: ReasonReact.self state retainedProps action) => {
  let code = self.ReasonReact.retainedProps.code;
  Utils.evalJs code (fun evalResult => self.reduce (fun _ => UpdateEvalResult evalResult) ())
};

let make ::code ::className=? _children => {
  ...component,
  retainedProps: {code: code},
  initialState: fun () => {evalResult: Utils.Success, evaluatingJs: false},
  reducer: fun action state =>
    switch action {
    | UpdateEvalResult evalResult => ReasonReact.Update {...state, evalResult, evaluatingJs: false}
    },
  didMount: fun self => {
    executeCode self;
    ReasonReact.NoUpdate
  },
  didUpdate: fun {oldSelf, newSelf} =>
    if (oldSelf.retainedProps.code !== newSelf.retainedProps.code) {
      executeCode newSelf
    },
  render: fun (self: ReasonReact.self state retainedProps action) => {
    let className = Js.Option.getWithDefault "" className;
    let errorMessage =
      switch self.state.evalResult {
      | Utils.Success => None
      | Utils.ErrorMessage e => Some e
      };
    <div className style=previewStyle>
      <div id="preview" />
      <Error errorMessage=?errorMessage />
    </div>
  }
};
