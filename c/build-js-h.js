const readline = require('readline');
const fs       = require('fs');
const print    = s => console.log(s);

function replace_type(type) {
    return type
        .replace("char*",  "string")
        .replace(/.+\*/g,  "number")
        .replace("void",   "undefined")
        .replace("byte",   "number")
        .replace("int",    "number")
        .replace("short",  "number")
        .replace("bool",   "number")
        .replace("float",  "number")
        .replace("double", "number");
}

var cc_exported = [];
var defs = [];
var funcs = [];

function add_def(line) {
    if (line.startsWith("#define") &&
        !line.includes("_LEN") &&
        !line.includes("_H_") &&
        !line.includes(") {")) {

        line = line.replace("#define", "var")
            .replace("{", "[").replace("}", "]")
            .split(" ");
        line.splice(2, 0, "=");
        line = line.join(" ") + ";";

        defs.push(line);
    }
}

function add_func(line) {
    if (line.endsWith(");")) {

        line = line.replace(/\s\s+/g, ' ').split(" ");
        var ret = replace_type(line.shift());
        var name = line[0].split("(");
        var params = line;
        params[0] = name[1];
        name = name[0];
        params[params.length - 1] = params[params.length - 1].replace(");", "");
        params = params.join(" ").split(", ");
        for (var i = 0; i < params.length; ++i) {
            params[i] = `"${replace_type(params[i].split(" ")[0])}"`;
        }
        cc_exported.push(`"_${name}"`);
        funcs.push(`var ${name} = Module.cwrap('${name}', '${ret}', [${params.join(',')}]);`);

    }
}

for (let line of fs.readFileSync("prayertimes.h", 'utf-8').split('\n')) {
    add_def(line);
    add_func(line);
}

cc_exported = `[${cc_exported.join(',')}]`;
defs = defs.join("\n");
funcs = funcs.join("\n");

fs.writeFileSync("js/cc-exported", cc_exported);
fs.writeFileSync("js/prayertimes.h.js", defs + '\n' + funcs);

