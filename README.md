# cordova-plugin-pagarme-mpos

A plugin for interacting with Pagar.me's mPOS libraries in Cordova hybrid applications.

## Platforms Supported
* iOS

## Installation Guide

1. Add Cordova plugin to project

``` sh
$ cordova plugin add cordova-plugin-pagarme-mpos
```

2. In your Xcode project, add the following to Project Settings -> Build Settings -> Other Linker Flags:
"-lstdc++ -stdlib=libstdc++"

3. In your project's Info.plist, be sure to add the appropriate "Supported external accessory protocols" for the POS you are using.

4. Build & Run.

## License
This project is licensed under the MIT license.
Licensing information may be found in COPYING.
