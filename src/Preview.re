/* Based on https://github.com/react-toolbox/react-toolbox/blob/dev/docs/app/components/preview/index.js */
type jsModule;

external reactModule : jsModule = "react" [@@bs.module];

external buttonModule : jsModule = "default" [@@bs.module "react-toolbox/lib/button/Button"];

external objGet : Js.t {..} => string => 'a = "" [@@bs.get_index];

type scopedFunction;

external evalToScopedFunction : string => scopedFunction = "eval" [@@bs.val];

external applyScope : scopedFunction => Js.null (Js.t {..}) => array 'a => ReasonReact.reactElement =
  "apply" [@@bs.send];

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
  error: option string,
  mountNode: ref (option Dom.element)
};

let handleRef elementRef self => self.ReasonReact.state.mountNode := Js.Null.to_opt elementRef;

type action =
  | EvalError (option string);

let component = ReasonReact.reducerComponentWithRetainedProps "Preview";

let executeCode self scope =>
  switch !self.ReasonReact.state.mountNode {
  | None => ()
  | Some mn =>
    let error =
      try {
        try (ReactDOMRe.unmountComponentAtNode mn) {
        | Js.Exn.Error e => Js.log (Js.Exn.message e)
        };
        let defaultScope = {"React": reactModule, "Button": buttonModule};
        let scope = Js.Option.getWithDefault defaultScope scope;
        let scopeArguments = Js.Obj.keys scope |> Js.Array.joinWith ", ";
        let code = self.ReasonReact.retainedProps.code;
        let functionToEvaluate = {j|(function ($(scopeArguments), mountNode) {
  $(code)
});|j};
        let evaluedFunction = evalToScopedFunction functionToEvaluate;
        let scopeValues =
          Js.Obj.keys scope |> Js.Array.map (fun k => objGet scope k) |> Js.Array.concat [|mn|];
        let reactElement = applyScope evaluedFunction Js.null scopeValues;
        ReactDOMRe.render reactElement mn;
        None
      } {
      | Js.Exn.Error e => Js.Exn.message e
      };
    self.reduce (fun _ => EvalError error) ()
  };

let make ::code ::className=? ::scope=? _children => {
  ...component,
  retainedProps: {code: code},
  initialState: fun () => {error: None, mountNode: ref None},
  reducer: fun action state =>
    switch action {
    | EvalError error => ReasonReact.Update {...state, error}
    },
  didMount: fun self => {
    executeCode self scope;
    ReasonReact.NoUpdate
  },
  didUpdate: fun {oldSelf, newSelf} =>
    if (oldSelf.retainedProps.code !== newSelf.retainedProps.code) {
      executeCode newSelf scope
    },
  render: fun self => {
    let className = Js.Option.getWithDefault "" className;
    <div className style=previewStyle>
      <div ref=(self.handle handleRef) style=contentStyle />
      <Error errorMessage=?self.state.error />
    </div>
  }
};
/*
 const Preview = React.createClass({
   componentDidUpdate (prevProps) {
     clearTimeout(this.timeoutID);
     if (this.props.code !== prevProps.code) {
       this.executeCode();
     }
   },

   setTimeout () {
     clearTimeout(this.timeoutID);
     this.timeoutID = setTimeout(...arguments);
   },

   executeCode () {
     const mountNode = this.refs.mount;
     const scope = this.buildScope(mountNode);

     try {
       ReactDOM.unmountComponentAtNode(mountNode);
     } catch (e) {
       console.error(e);
     }

     try {
       ReactDOM.render(eval(this.compileCode())(...scope), mountNode);
       if (this.state.error) {
         this.setState({ error: null });
       }
     } catch (err) {
       this.setTimeout(() => {
         this.setState({ error: err.toString() });
       }, ERROR_TIMEOUT);
     }
   },

   render () {
     let className = style.preview;
     if (this.props.className) className += ` ${this.props.className}`;

     return (
       <div className={className}>
         {this.state.error !== null ? <span className={style.error}>{this.state.error}</span> : null}
         <div ref="mount" className={style.content} />
       </div>
     );
   }
 });

 export default Preview;
 */
