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
  initialState: fun () => {
    evalResult: Utils.ReactElement ReasonReact.nullElement,
    evaluatingJs: false
  },
  reducer: fun action state =>
    switch action {
    | UpdateEvalResult evalResult => ReasonReact.Update {...state, evalResult, evaluatingJs: true}
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
    let reactElement =
      switch self.state.evalResult {
      | Utils.ReactElement re => re
      | Utils.ErrorMessage _ => ReasonReact.nullElement
      };
    let errorMessage =
      switch self.state.evalResult {
      | Utils.ReactElement _
      | Utils.ErrorMessage "" => None
      | Utils.ErrorMessage e => Some e
      };
    <div className style=previewStyle> reactElement <Error errorMessage=?errorMessage /> </div>
  }
};
