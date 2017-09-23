/*
 import React from 'react';
 import ReactDOM from 'react-dom';
 import * as ReactToolbox from 'react-toolbox';
 */
type jsModule;

external reactModule : jsModule = "react" [@@bs.module];

external buttonModule : jsModule = "default" [@@bs.module "react-toolbox/lib/button/Button"];

external objGet : Js.t {..} => string => 'a = "" [@@bs.get_index];

external eval : string => 'a = "" [@@bs.val];

external apply : 'a => Js.null (Js.t {..}) => array 'a => ReasonReact.reactElement =
  "" [@@bs.send];

let errorStyle =
  ReactDOMRe.Style.make
    backgroundColor::"#faa"
    borderBottom::"1px solid"
    color::"black"
    fontFamily::"monospace"
    fontSize::"14pt"
    lineHeight::"14px"
    padding::"5px 10px"
    ();

let contentStyle = ReactDOMRe.Style.make flexGrow::"1" margin::"10px" ();

type retainedProps = {code: string};

type state = {
  error: option string,
  mountNode: ref (option Dom.element)
};

let handleRef elementRef self => self.ReasonReact.state.mountNode := Js.Null.to_opt elementRef;

type action =
  | EvalError string;

let component = ReasonReact.reducerComponentWithRetainedProps "Preview";

let compile scope self =>
  switch !self.ReasonReact.state.mountNode {
  | None => ()
  | Some mn =>
    ReactDOMRe.unmountComponentAtNode mn;
    let scope = Js.Option.getWithDefault {"React": reactModule, "Button": buttonModule} scope;
    let scopeArguments = Js.Obj.keys scope |> Js.Array.joinWith ", ";
    let code = self.ReasonReact.retainedProps.code;
    let functionToEvaluate = {j|(function ($(scopeArguments), mountNode) {
      $(code)
    });|j};
    let evaluedFunction = eval functionToEvaluate;
    let scopeValues =
      Js.Obj.keys scope |> Js.Array.map (fun k => objGet scope k) |> Js.Array.concat [|mn|];
    let reactElement = apply evaluedFunction Js.null scopeValues;
    ReactDOMRe.render reactElement mn
  };

let make ::code ::className=? ::scope=? _children => {
  ...component,
  retainedProps: {code: code},
  initialState: fun () => {error: None, mountNode: ref None},
  reducer: fun action state =>
    switch action {
    | EvalError error => ReasonReact.Update {...state, error: Some error}
    },
  didMount: fun self => {
    compile scope self;
    ReasonReact.NoUpdate
  },
  didUpdate: fun {oldSelf, newSelf} =>
    if (oldSelf.retainedProps.code !== newSelf.retainedProps.code) {
      compile scope newSelf
    },
  render: fun self => {
    let className = Js.Option.getWithDefault "" className;
    let errorElement =
      switch self.state.error {
      | None => ReasonReact.nullElement
      | Some e => <span style=errorStyle> (ReasonReact.stringToElement e) </span>
      };
    <div className> errorElement <div ref=(self.handle handleRef) style=contentStyle /> </div>
  }
};
/*
 const Preview = React.createClass({
   componentDidMount () {
     this.executeCode();
   },

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

   compileCode () {
     const code = `
       (function (${Object.keys(this.props.scope).join(', ')}, mountNode) {
         ${this.props.code}
       });`;

     return transform(code, {
       presets: ['es2015', 'stage-0', 'react']
     }).code;
   },

   buildScope (mountNode) {
     return Object.keys(this.props.scope).map(key => this.props.scope[key]).concat(mountNode);
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
