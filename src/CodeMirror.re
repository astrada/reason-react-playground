[%bs.raw {|require('codemirror/lib/codemirror.css')|}];

[@bs.module] external reactClass : ReasonReact.reactClass = "@skidding/react-codemirror";

[%bs.raw {|require('codemirror/mode/javascript/javascript')|}];

[%bs.raw {|require('codemirror/mode/mllike/mllike')|}];

[%bs.raw {|require('codemirror/mode/rust/rust')|}];

type scrollInfo = {
  .
  "left": float,
  "top": float,
  "width": float,
  "height": float,
  "clientWidth": float,
  "clientHeight": float
};

type position = {. "line": float, "ch": float};

type change = {
  .
  "from": position, "_to": position, "text": array(string), "removed": string, "origin": string
};

let make =
    (
      ~autoFocus: option(bool)=?,
      ~autoSave: option(bool)=?,
      ~className: option(string)=?,
      ~defaultValue: option(string)=?,
      ~options: option(Js.t({..}))=?,
      ~onChange: option(((string, change) => unit))=?,
      ~onCursorActivity: option(ReasonReact.Callback.t(Js.t({..})))=?,
      ~onFocusChange: option(ReasonReact.Callback.t(Js.boolean))=?,
      ~onScroll: option(ReasonReact.Callback.t(scrollInfo))=?,
      ~preservePositionScroll: option(bool)=?,
      ~value: option(string)=?,
      children
    ) =>
  ReasonReact.wrapJsForReason(
    ~reactClass,
    ~props={
      "autoFocus": Js.Nullable.from_opt(Utils.optionMap(Js.Boolean.to_js_boolean, autoFocus)),
      "autoSave": Js.Nullable.from_opt(Utils.optionMap(Js.Boolean.to_js_boolean, autoSave)),
      "className": Js.Nullable.from_opt(className),
      "defaultValue": Js.Nullable.from_opt(defaultValue),
      "options": Js.Nullable.from_opt(options),
      "onChange": Js.Nullable.from_opt(onChange),
      "onCursorActivity": Js.Nullable.from_opt(onCursorActivity),
      "onFocusChange": Js.Nullable.from_opt(onFocusChange),
      "onScroll": Js.Nullable.from_opt(onScroll),
      "preservePositionScroll":
        Js.Nullable.from_opt(Utils.optionMap(Js.Boolean.to_js_boolean, preservePositionScroll)),
      "value": Js.Nullable.from_opt(value)
    },
    children
  );
