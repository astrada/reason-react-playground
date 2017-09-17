let component = ReasonReact.statelessComponent "CodeEditor";

let options = {"lineNumbers": Js.Boolean.to_js_boolean true};

let rowStyle =
  ReactDOMRe.Style.make
    flex::"1"
    minHeight::"0"
    border::"1px solid #aaa"
    position::"relative"
    overflow::"auto"
    margin::"20px 0 20px 0"
    ();

let labelStyle =
  ReactDOMRe.Style.make
    position::"absolute"
    top::"0"
    right::"0"
    backgroundColor::"rgba(200, 200, 200, 0.6)"
    padding::"5px 10px"
    fontWeight::"bold"
    fontFamily::"monospace"
    color::"black"
    fontSize::"14pt"
    lineHeight::"14px"
    flexDirection::"row"
    alignItems::"center"
    zIndex::"20"
    borderRadius::"0 0 0 5px"
    ();

let make ::label ::code=? ::onChange=? _children => {
  ...component,
  render: fun _self =>
    <div style=rowStyle>
      <div style=labelStyle> (ReasonReact.stringToElement label) </div>
      <CodeMirror value=?code onChange=?onChange options />
    </div>
};
