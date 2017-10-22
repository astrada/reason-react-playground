ReasonReact Playground
----------------------

ReasonReact with editable source and live preview.

How it works
------------

Going from ReasonML to JavaScript requires 3 transformation steps:

1. Transform ReasonML to OCaml+JSX
2. Transform OCaml+JSX to plain OCaml
3. Compile OCaml to JS

#### Step 1

ReasonML code is transformed to OCaml+JSX by
[refmt-js](https://github.com/reasonml-community/refmt-js).

#### Step 2

JSX syntax is handled by the version of
[reactjs_jsx_ppx_v2](https://github.com/BuckleScript/bucklescript/blob/master/jscomp/bin/reactjs_jsx_ppx_v2.bspp.ml)
recently landed in BuckleScript master branch. This version can be
[compiled](https://github.com/BuckleScript/bucklescript/blob/master/jscomp/repl.js#L60)
by [js_of_ocaml](https://github.com/ocsigen/js_of_ocaml/) to be used in a web
browser.

It exposes a `jsxv2` object with a `rewrite` method to perform the
transformation.

For example:
```ocaml
(button ~children:[ReasonReact.stringToElement "Hello!"] ()) [@JSX ]
```
Gets transformed to:
```ocaml
let _ =
  ReactDOMRe.createElement "button"
    [|(ReasonReact.stringToElement "Hello!")|]
```
If there are no errors in the input string, the output code is returned in a serialized JSON object, e.g.:
```js
jsxv2.rewrite('(button ~children:[ReasonReact.stringToElement "Hello!"] ()) [@JSX ]')
```
Returns:
```json
{
    "ocaml_code": "let _ =\n  ReactDOMRe.createElement \"button\"\n    [|(ReasonReact.stringToElement \"Hello!\")|]"
}
```
Otherwise, you will get a serialized JSON object containing the error returned by the PPX preprocessor, e.g.:
```js
jsxv2.rewrite('(button () /) [@JSX]')
```
Returns:
```json
{
    "ppx_error_msg": "Line 1, 12: Syntax error: operator expected.",
    "row": 0,
    "column": 12,
    "endRow": 0,
    "endColumn": 13,
    "text": "Syntax error: operator expected.",
    "type": "error"
}
```

#### Step 3

Pure OCaml code returned by Step 2 is compiled to JavaScript using the [JS
version](https://github.com/BuckleScript/bucklescript-playground/blob/master/exports.js)
of BuckleScript (also compiled by
[js_of_ocaml](https://github.com/ocsigen/js_of_ocaml/)).

To load external modules, you can use the new `load_module` method
[recently](https://github.com/BuckleScript/bucklescript/blob/master/jscomp/core/jsoo_main.ml#L180)
added to the JS API of BuckleScript. This methods loads an external module
given it's compiled `.cmi` and `.cmj` contents. It accepts these parameters:
  * `cmi_path`: `.cmi` path in the (JSOO) pseudo file-system
  * `cmi_content`: binary serialization of the `.cmi` file
  * `cmj_name`: `.cmj` filename
  * `cmj_content`: binary serialization of the `.cmj` file

See [this
file](https://github.com/astrada/reason-react-playground/blob/source/compiler/externalModules.js)
for an example of how to load ReasonReact modules.

`.cmi` and `.cmj` contents can be serialized using [this
utility](https://github.com/astrada/reason-react-playground/blob/source/compiler/bin2js.js).
See [this
script](https://github.com/astrada/reason-react-playground/blob/source/compiler/regenerateExternalModules.js)
for an example of how to use `bin2js` to generate `externalModules.js`.

JavaScript code produced by BuckleScript needs React and ReasonReact to be
evaluated, so we need to bundle those libraries with browserify (as it's done
for the stdlib) using this
[script](https://github.com/astrada/reason-react-playground/blob/source/compiler/regenerateReasonReactForBrowser.js).

Acknowledgments
---------------

Based on:
  * [ReasonML playground](https://reasonml.github.io/try/)
  * [BuckleScript
    playground](https://github.com/BuckleScript/bucklescript-playground)
  * [react-toolbox.com](http://react-toolbox.com/)

Thanks to [Hongbo Zhang](https://github.com/bobzhang) and to [Cheng
Lou](https://github.com/chenglou) for reviewing my patches. Thanks to the
[Ocsigen team](https://github.com/ocsigen/) for developing js_of_ocaml.

