const path                 = require("path");
const fs                   = require("fs");
const webpack              = require("webpack");
const HtmlWebpackPlugin    = require("html-webpack-plugin");
const WebpackPwaManifest   = require("webpack-pwa-manifest");
const BundleAnalyzerPlugin = require('webpack-bundle-analyzer').BundleAnalyzerPlugin;

function buildString() {
    let d = new Date();
    let ret = d.getFullYear() +
        d.getMonth().toString().padStart(2, '0') +
        d.getDate().toString().padStart(2, '0') +
        "-" +
        d.getHours().toString().padStart(2, '0') +
        d.getMinutes().toString().padStart(2, '0');
    return JSON.stringify(ret);
}

module.exports = env => {
    return {
        target:  ["node", "es3"],
        mode:    env.production ? "production" : "development",
        devtool: env.production ? undefined : "inline-source-map",

        entry: {
            index: "./main.js"
        },

        output: {
            path:     path.resolve(__dirname, "../build/web"),
            filename: "[name].bundle.js",
            clean:    false
        },

        resolve: {
            extensions: [".js"],
        },

        module: {
            rules: [
                {test: /\.js$/, loader: "ts-loader"},
                {test: /\.css$/, use: ["style-loader", "css-loader"]},
                { test: /\.ttf$/, loader: "url-loader"}
            ]
        },

        plugins: [
            new webpack.DefinePlugin({
                BUILD: buildString(),
            }),
            
            new HtmlWebpackPlugin({
                template: "./index.html",
                chunks:   ["index"]
            }),

            /*new WebpackPwaManifest({
                name:             "Prayer",
                short_name:       "Prayer",
                description:      "Muslim prayer times, by Naheel",
                background_color: "black",
                display:          "standalone",
                orientation:      "omit",
                fingerprints:     false,
                icons: [
                    {
                        src: path.resolve("../icon.png"),
                        sizes: [96, 128, 192, 256, 384, 512]
                    },
                    {
                        src: path.resolve("../icon.png"),
                        size: "512x512",
                        purpose: "maskable"
                    }
                ]
            }),*/

            //new BundleAnalyzerPlugin()
        ]
    };
};
