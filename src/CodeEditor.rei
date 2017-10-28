let rowStyle: ReactDOMRe.style;

let labelStyle: ReactDOMRe.style;

let make:
  label::string =>
  mode::'a =>
  loading::bool? =>
  code::string? =>
  error::string? =>
  onChange::(string => CodeMirror.change => unit)? =>
  readOnly::bool? =>
  array ReasonReact.reactElement =>
  ReasonReact.componentSpec
    ReasonReact.stateless
    ReasonReact.stateless
    ReasonReact.noRetainedProps
    ReasonReact.noRetainedProps
    ReasonReact.actionless;
