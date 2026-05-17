// Copyright (c) Chris Hafey.
// SPDX-License-Identifier: MIT
// BUILD_VERSION: 2026-05-17-debug-logging

#include "J2KDecoder.hpp"
#include "FrameInfo.hpp"
#include "Point.hpp"
#include "Size.hpp"

#include <emscripten.h>
#include <emscripten/bind.h>
#include <stdio.h>

using namespace emscripten;

// ---------------------------------------------------------------------------
// Debug-instrumented thin wrappers around J2KDecoder
// These emit console-visible printf output so we can trace the exact point
// of failure when the WASM trap occurs.
// ---------------------------------------------------------------------------

struct DebugJ2KDecoder {
  J2KDecoder inner;

  emscripten::val getEncodedBuffer(size_t encodedSize) {
    printf("[DBG] getEncodedBuffer(%zu)\n", encodedSize);
    auto result = inner.getEncodedBuffer(encodedSize);
    printf("[DBG] getEncodedBuffer OK\n");
    return result;
  }

  emscripten::val getDecodedBuffer() {
    printf("[DBG] getDecodedBuffer()\n");
    auto result = inner.getDecodedBuffer();
    printf("[DBG] getDecodedBuffer OK\n");
    return result;
  }

  void readHeader() {
    printf("[DBG] readHeader()\n");
    inner.readHeader();
    printf("[DBG] readHeader OK\n");
  }

  Size calculateSizeAtDecompositionLevel(int decompositionLevel) {
    printf("[DBG] calculateSizeAtDecompositionLevel(%d)\n", decompositionLevel);
    auto result = inner.calculateSizeAtDecompositionLevel(decompositionLevel);
    printf("[DBG] calculateSizeAtDecompositionLevel OK -> %dx%d\n", result.width, result.height);
    return result;
  }

  void decode() {
    printf("[DBG] decode() - about to call openjpeg\n");
    inner.decode();
    printf("[DBG] decode() OK\n");
  }

  void decodeSubResolution(int decompositionLevel, int quality) {
    printf("[DBG] decodeSubResolution(level=%d, quality=%d)\n", decompositionLevel, quality);
    inner.decodeSubResolution(decompositionLevel, quality);
    printf("[DBG] decodeSubResolution OK\n");
  }

  FrameInfo getFrameInfo() {
    printf("[DBG] getFrameInfo()\n");
    auto fi = inner.getFrameInfo();
    printf("[DBG] getFrameInfo OK -> %dx%d bpp=%d comps=%d signed=%d\n",
           fi.width, fi.height, fi.bitsPerSample, fi.componentCount, (int)fi.isSigned);
    return fi;
  }

  int getNumDecompositions()  { return inner.getNumDecompositions(); }
  bool getIsReversible()      { return inner.getIsReversible(); }
  int getProgressionOrder()   { return inner.getProgressionOrder(); }
  Point getImageOffset()      { return inner.getImageOffset(); }
  Size getTileSize()          { return inner.getTileSize(); }
  Point getTileOffset()       { return inner.getTileOffset(); }
  Size getBlockDimensions()   { return inner.getBlockDimensions(); }
  int getNumLayers()          { return inner.getNumLayers(); }
  int getColorSpace()         { return inner.getColorSpace(); }
};

EMSCRIPTEN_BINDINGS(FrameInfo) {
  value_object<FrameInfo>("FrameInfo")
    .field("width", &FrameInfo::width)
    .field("height", &FrameInfo::height)
    .field("bitsPerSample", &FrameInfo::bitsPerSample)
    .field("componentCount", &FrameInfo::componentCount)
    .field("isSigned", &FrameInfo::isSigned)
       ;
}

EMSCRIPTEN_BINDINGS(Point) {
  value_object<Point>("Point")
    .field("x", &Point::x)
    .field("y", &Point::y)
       ;
}

EMSCRIPTEN_BINDINGS(Size) {
  value_object<Size>("Size")
    .field("width", &Size::width)
    .field("height", &Size::height)
       ;
}

EMSCRIPTEN_BINDINGS(J2KDecoder) {
  class_<DebugJ2KDecoder>("J2KDecoder")
    .constructor<>()
    .function("getEncodedBuffer",                 &DebugJ2KDecoder::getEncodedBuffer)
    .function("getDecodedBuffer",                 &DebugJ2KDecoder::getDecodedBuffer)
    .function("readHeader",                       &DebugJ2KDecoder::readHeader)
    .function("calculateSizeAtDecompositionLevel",&DebugJ2KDecoder::calculateSizeAtDecompositionLevel)
    .function("decode",                           &DebugJ2KDecoder::decode)
    .function("decodeSubResolution",              &DebugJ2KDecoder::decodeSubResolution)
    .function("getFrameInfo",                     &DebugJ2KDecoder::getFrameInfo)
    .function("getNumDecompositions",             &DebugJ2KDecoder::getNumDecompositions)
    .function("getIsReversible",                  &DebugJ2KDecoder::getIsReversible)
    .function("getProgressionOrder",              &DebugJ2KDecoder::getProgressionOrder)
    .function("getImageOffset",                   &DebugJ2KDecoder::getImageOffset)
    .function("getTileSize",                      &DebugJ2KDecoder::getTileSize)
    .function("getTileOffset",                    &DebugJ2KDecoder::getTileOffset)
    .function("getBlockDimensions",               &DebugJ2KDecoder::getBlockDimensions)
    .function("getNumLayers",                     &DebugJ2KDecoder::getNumLayers)
    .function("getColorSpace",                    &DebugJ2KDecoder::getColorSpace)
   ;
}
