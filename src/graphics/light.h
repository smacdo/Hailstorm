/*
 * Copyright 2013 Scott MacDonald
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
#ifndef SCOTT_HAILSTORM_LIGHT_H
#define SCOTT_HAILSTORM_LIGHT_H

#include <D3DX10.h>

/**
 * Defines light information
 */
struct Light
{
	Light();
	~Light();

	D3DXVECTOR3 pos;
	float unused1;		// padding
	D3DXVECTOR3 dir;
	float unused2;		// padding
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	D3DXVECTOR3 att;
	float spotPow;
	float range;
};

#endif