#include <nan.h>
#include <iostream>

#include "Utils.hpp"

std::Utils utils;

std::string convertNodeStringToString(const Nan::FunctionCallbackInfo<v8::Value>& info, int position){
    v8::String::Utf8Value fileLocationNode(info[position]);
    std::string fileLocation(*fileLocationNode);
    return fileLocation;
}

cv::Rect convertRectObjToRect(v8::Local<v8::Object> retangle){
    int width = 0, height = 0, x = 0, y = 0;

    if(Nan::Has(retangle, Nan::New("width").ToLocalChecked()).IsJust())
        width = Nan::Get(retangle, Nan::New("width").ToLocalChecked()).ToLocalChecked()->NumberValue();

    if (Nan::Has(retangle, Nan::New("height").ToLocalChecked()).IsJust())
        height = Nan::Get(retangle, Nan::New("height").ToLocalChecked()).ToLocalChecked()->NumberValue();

    if (Nan::Has(retangle, Nan::New("x").ToLocalChecked()).IsJust())
        x = Nan::Get(retangle, Nan::New("x").ToLocalChecked()).ToLocalChecked()->NumberValue();

    if (Nan::Has(retangle, Nan::New("y").ToLocalChecked()).IsJust())
        y = Nan::Get(retangle, Nan::New("y").ToLocalChecked()).ToLocalChecked()->NumberValue();

    return cv::Rect(height, width, x, y);
}

RectsSimple convertArrayRecsToRectsSimple(v8::Local<v8::Array> retangles)
{
    RectsSimple retanglesNative;
    for (int i = 0; i < static_cast<int>(retangles->Length()); i++)
    {
        retanglesNative.push_back(convertRectObjToRect(retangles->Get(i)->ToObject()));
    }

    return retanglesNative;
}

v8::Local<v8::Object> convertRectToObject(cv::Rect retangulo)
{
    v8::Local<v8::Object> retanguloObjectNative = Nan::New<v8::Object>();
    retanguloObjectNative->Set(Nan::New("width").ToLocalChecked(), Nan::New<v8::Number>(retangulo.width));
    retanguloObjectNative->Set(Nan::New("height").ToLocalChecked(), Nan::New<v8::Number>(retangulo.height));
    retanguloObjectNative->Set(Nan::New("x").ToLocalChecked(), Nan::New<v8::Number>(retangulo.x));
    retanguloObjectNative->Set(Nan::New("y").ToLocalChecked(), Nan::New<v8::Number>(retangulo.y));

    return retanguloObjectNative;
}

v8::Local<v8::Array> convertRectListToArray(RectsSimple retangulos){
    v8::Local<v8::Array> retangulosLocalArray = Nan::New<v8::Array>();
    for (int i = 0; i < static_cast<int>(retangulos.size()); i++){
        v8::Local<v8::Object> retangleObj = convertRectToObject(retangulos[i]);
        retangulosLocalArray->Set(i, retangleObj);
    }
    return retangulosLocalArray;
}

bool assetFunctionCountParameters(const Nan::FunctionCallbackInfo<v8::Value>& info, int parameters){
    if(info.Length() <= parameters){
        Nan::ThrowTypeError("Error in number of argument");
        return false;
    }else{
        return true;
    }
}

void setThreshFindSquare(const Nan::FunctionCallbackInfo<v8::Value>& info){
    if(!assetFunctionCountParameters(info, 0)){
        return;
    }

    if(!info[0]->IsNumber()){
        Nan::ThrowTypeError("First argument not a number");
    }

    double arg0 = info[0]->NumberValue();

    utils.setThresh(static_cast<int>(arg0));
}


void getThreshFindSquare(const Nan::FunctionCallbackInfo<v8::Value>& info){
    v8::Local<v8::Number> number = Nan::New(utils.getThresh());
    info.GetReturnValue().Set(number);
}

void setNThresholdLevel(const Nan::FunctionCallbackInfo<v8::Value>& info){
    if (!assetFunctionCountParameters(info, 0))
    {
        return;
    }

    if (!info[0]->IsNumber())
    {
        Nan::ThrowTypeError("First argument not a number");
    }

    double arg0 = info[0]->NumberValue();

    utils.setN(static_cast<int>(arg0));
}

void getNThresholdLevel(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Local<v8::Number> number = Nan::New(utils.getN());
    info.GetReturnValue().Set(number);
}

void setThreshOfMescleRetct(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    if (!assetFunctionCountParameters(info, 0))
    {
        return;
    }

    if (!info[0]->IsNumber())
    {
        Nan::ThrowTypeError("First argument not a number");
        return;
    }

    double arg0 = info[0]->NumberValue();
    
    utils.setThreshOfMescleRetct(static_cast<int>(arg0));
}

void getThreshOfMescleRetct(const Nan::FunctionCallbackInfo<v8::Value> &info)
{
    v8::Local<v8::Number> number = Nan::New(utils.getThreshOfMescleRetct());
    info.GetReturnValue().Set(number);
}

void getSquare(const Nan::FunctionCallbackInfo<v8::Value> &info){
    if (!assetFunctionCountParameters(info, 0))
    {
        return;
    }

    if (!info[0]->IsString())
    {
        Nan::ThrowTypeError("First argument not a string");
        return;
    }

    int width = 0, height = 0, x = 0, y = 0;
    bool delimitationActive = false;

    if(!info[1]->IsObject()){
        delimitationActive = false;
    }else{
        delimitationActive = true;
        v8::Local<v8::Object> params = info[1]->ToObject();
        if (Nan::Has(params, Nan::New("width").ToLocalChecked()).IsJust())
            width = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->NumberValue();

        if (Nan::Has(params, Nan::New("height").ToLocalChecked()).IsJust())
            height = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->NumberValue();

        if (Nan::Has(params, Nan::New("x").ToLocalChecked()).IsJust())
            x = Nan::Get(params, Nan::New("x").ToLocalChecked()).ToLocalChecked()->NumberValue();

        if (Nan::Has(params, Nan::New("y").ToLocalChecked()).IsJust())
            y = Nan::Get(params, Nan::New("y").ToLocalChecked()).ToLocalChecked()->NumberValue();
    }

    std::string fileLocation = convertNodeStringToString(info, 0);

    // Execution

    cv::Mat image = cv::imread(fileLocation);
    cv::Rect delimitation(x, y, width, height);

    RectsSimple result;
    cv::Rect delimited;

    utils.getRetanglesByImage(image, delimitationActive, delimitation, result, delimited);

    v8::Local<v8::Object> obj = Nan::New<v8::Object>();
    obj->Set(Nan::New("delimited").ToLocalChecked(), convertRectToObject(delimited));
    obj->Set(Nan::New("rectangles").ToLocalChecked(), convertRectListToArray(result));

    info.GetReturnValue().Set(obj);
}

void getPercentOfRetangles(const Nan::FunctionCallbackInfo<v8::Value> &info){
    int width = 0, height = 0, x = 0, y = 0;
    bool delimitationActive = false;
    bool paramRetangles = false;

    RectsSimple paramRetangle;

    cv::Mat image;

    if (!assetFunctionCountParameters(info, 0))
    {
        return;
    }

    if (info[0]->IsString())
    {
        std::string fileLocation = convertNodeStringToString(info, 0);
        image = cv::imread(fileLocation);
    }else{
        Nan::ThrowTypeError("First argument not a string or undefined");
        return;
    }

    if(!info[1]->IsObject()){
        delimitationActive = false;
    }else if(info[1]->IsArray()){
        paramRetangles = true;
        paramRetangle = convertArrayRecsToRectsSimple(v8::Local<v8::Array>::Cast(info[1]));
    }else{
        delimitationActive = true;
        v8::Local<v8::Object> params = info[1]->ToObject();
        if (Nan::Has(params, Nan::New("width").ToLocalChecked()).IsJust())
            width = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->NumberValue();

        if (Nan::Has(params, Nan::New("height").ToLocalChecked()).IsJust())
            height = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->NumberValue();

        if (Nan::Has(params, Nan::New("x").ToLocalChecked()).IsJust())
            x = Nan::Get(params, Nan::New("x").ToLocalChecked()).ToLocalChecked()->NumberValue();

        if (Nan::Has(params, Nan::New("y").ToLocalChecked()).IsJust())
            y = Nan::Get(params, Nan::New("y").ToLocalChecked()).ToLocalChecked()->NumberValue();
    }

    if(!info[2]->IsArray()){
        paramRetangles = false;
    }else{
        paramRetangles = true;
        paramRetangle = convertArrayRecsToRectsSimple(v8::Local<v8::Array>::Cast(info[2]));
    }
    
    cv::Rect delimitation(x, y, width, height);

    if(paramRetangles){
        info.GetReturnValue().Set(Nan::New<v8::Number>(utils.calcAreaOfAllRetangle(paramRetangle)));
    }else{
        info.GetReturnValue().Set(Nan::New<v8::Number>(utils.getRetanglesPercent(image, delimitationActive, delimitation)));
    }
}


void Init(v8::Local<v8::Object> exports){

    exports->Set(Nan::New("setThreshFindSquare").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setThreshFindSquare)->GetFunction());
    exports->Set(Nan::New("getThreshFindSquare").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getThreshFindSquare)->GetFunction());

    exports->Set(Nan::New("setNThresholdLevel").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setNThresholdLevel)->GetFunction());
    exports->Set(Nan::New("getNThresholdLevel").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getNThresholdLevel)->GetFunction());

    exports->Set(Nan::New("setThreshOfMescleRetct").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setThreshOfMescleRetct)->GetFunction());
    exports->Set(Nan::New("getThreshOfMescleRetct").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getThreshOfMescleRetct)->GetFunction());

    exports->Set(Nan::New("getSquare").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSquare)->GetFunction());
    exports->Set(Nan::New("getPercentOfRetangles").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getPercentOfRetangles)->GetFunction());
}

NODE_MODULE(brocken, Init);