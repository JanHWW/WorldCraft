#pragma once
constexpr int VerticeStride3D = (3 + 2 + 3);
constexpr int CubePlaneCount = 6;
constexpr int PlaneVerticeCount = 4;
constexpr int PlaneTriangleCount = 2;
constexpr int PlaneStride3D = PlaneVerticeCount * VerticeStride3D;
constexpr int CubeTriangleCount = CubePlaneCount * PlaneTriangleCount;
constexpr int CubeVerticeCount = CubePlaneCount * PlaneVerticeCount;
constexpr int CubeStride3D = CubeVerticeCount * VerticeStride3D;
constexpr int TriangleVerticeCount = 3;