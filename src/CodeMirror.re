[%bs.raw {|require('codemirror/lib/codemirror.css')|}];

external reactClass : ReasonReact.reactClass = "react-codemirror" [@@bs.module];

[%bs.raw {|require('codemirror/mode/mllike/mllike')|}];

[%bs.raw {|require('codemirror/mode/rust/rust')|}];

type scrollInfo =
  Js.t {
    .
    left : float,
    top : float,
    width : float,
    height : float,
    clientWidth : float,
    clientHeight : float
  };

type position = Js.t {. line : float, ch : float};

type change =
  Js.t {. from : position, _to : position, text : array string, removed : string, origin : string};

let make
    autoFocus::(autoFocus: option bool)=?
    autoSave::(autoSave: option bool)=?
    className::(className: option string)=?
    defaultValue::(defaultValue: option string)=?
    options::(options: option (Js.t {..}))=?
    onChange::(onChange: option (string => change => unit))=?
    onCursorActivity::(onCursorActivity: option (ReasonReact.Callback.t (Js.t {..})))=?
    onFocusChange::(onFocusChange: option (ReasonReact.Callback.t Js.boolean))=?
    onScroll::(onScroll: option (ReasonReact.Callback.t scrollInfo))=?
    preservePositionScroll::(preservePositionScroll: option bool)=?
    value::(value: option string)=?
    children =>
  ReasonReact.wrapJsForReason
    ::reactClass
    props::{
      "autoFocus": Js.Nullable.from_opt (Utils.optionMap Js.Boolean.to_js_boolean autoFocus),
      "autoSave": Js.Nullable.from_opt (Utils.optionMap Js.Boolean.to_js_boolean autoSave),
      "className": Js.Nullable.from_opt className,
      "defaultValue": Js.Nullable.from_opt defaultValue,
      "options": Js.Nullable.from_opt options,
      "onChange": Js.Nullable.from_opt onChange,
      "onCursorActivity": Js.Nullable.from_opt onCursorActivity,
      "onFocusChange": Js.Nullable.from_opt onFocusChange,
      "onScroll": Js.Nullable.from_opt onScroll,
      "preservePositionScroll":
        Js.Nullable.from_opt (Utils.optionMap Js.Boolean.to_js_boolean preservePositionScroll),
      "value": Js.Nullable.from_opt value
    }
    children;
