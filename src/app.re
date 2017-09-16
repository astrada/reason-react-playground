[%bs.raw {|require('./toolbox/theme.css')|}];

external theme : ReactToolbox.ThemeProvider.theme = "./toolbox/theme" [@@bs.module];

type jsCompilerResult =
  | JsCode string
  | JsErrorMessage string;

type state = {
  reasonCode: string,
  jsCompilerResult,
  compiling: bool
};

type action =
  | UpdateReasonCode string
  | UpdateJsCompilerResult jsCompilerResult
  | Compile;

let component = ReasonReact.reducerComponent "App";

let make _children => {
  ...component,
  initialState: fun () => {reasonCode: "", jsCompilerResult: JsCode "", compiling: false},
  reducer: fun action state =>
    switch action {
    | UpdateReasonCode reasonCode => ReasonReact.Update {...state, reasonCode}
    | UpdateJsCompilerResult jsCompilerResult =>
      ReasonReact.Update {...state, compiling: false, jsCompilerResult}
    | Compile => ReasonReact.Update {...state, compiling: true}
    },
  render: fun self => {
    let logo = <Logo />;
    let onChange code event => {
      let reduce = self.reduce (fun _event => UpdateReasonCode code);
      reduce event
    };
    let compile event => {
      let worker = Worker.make "compilerWorker.js";
      Worker.onMessage
        worker
        (
          fun messageEvent => {
            let result = CompilerWorker.toCompilerResult messageEvent##data;
            let jsCode = Js.Nullable.to_opt result##js_code;
            let jsErrorMessage =
              Js.Option.firstSome
                (Js.Nullable.to_opt result##js_error_msg) (Js.Nullable.to_opt result##message) |>
              Js.Option.getWithDefault "";
            let jsCompilerResult =
              if (Js.Option.isSome jsCode) {
                JsCode (Js.Option.getExn jsCode)
              } else {
                JsErrorMessage jsErrorMessage
              };
            let reduce = self.reduce (fun _event => UpdateJsCompilerResult jsCompilerResult);
            reduce event
          }
        );
      Worker.postMessage worker self.state.reasonCode;
      let reduce = self.reduce (fun _event => Compile);
      reduce event
    };
    let (jsCode, jsErrorMessage) =
      switch self.state.jsCompilerResult {
      | JsCode jsCode => (jsCode, "")
      | JsErrorMessage jsErrorMessage => ("", jsErrorMessage)
      };
    <ReactToolbox.ThemeProvider theme>
      <div>
        <ReactToolbox.AppBar title="App example" leftIcon=logo />
        <section style=(ReactDOMRe.Style.make padding::"20px" ())>
          <ReactToolbox.Input
            _type="text"
            multiline=true
            label=(ReasonReact.stringToElement "ReasonML")
            value=self.state.reasonCode
            onChange
          />
          <ReactToolbox.Button
            label="Compile"
            primary=true
            raised=true
            onClick=compile
            disabled=self.state.compiling
          />
          <ReactToolbox.Input
            _type="text"
            multiline=true
            label=(ReasonReact.stringToElement "JS")
            value=(jsCode ^ jsErrorMessage)
          />
        </section>
      </div>
    </ReactToolbox.ThemeProvider>
  }
};
