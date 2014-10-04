/*
 * Copyright 2011 Scott MacDonald
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SCOTT_HAILSTORM_GRAPHICS_STATIC_MESH_VERTEX
#define SCOTT_HAILSTORM_GRAPHICS_STATIC_MESH_VERTEX

#include <d3dx10.h>

/**
 * The vertex structure that is used in the static mesh
 */
struct StaticMeshVertex
{
    D3DXVECTOR3 pos;
    D3DXCOLOR color;
};

#endif