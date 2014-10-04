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
#include "stdafx.h"
#include "graphics/graphicscontentmanager.h"

#include <DXGI.h>
#include <d3d10.h>
#include <d3dx10.h>

#include "graphics/dxutils.h"
#include "graphics/meshfactory.h"

/**
 * Graphics content manager constructor
 */
GraphicsContentManager::GraphicsContentManager( ID3D10Device *pRenderDevice,
                                                const std::string& contentDir )
    : mContentDir( contentDir ),
      mMeshFactory( contentDir, pRenderDevice )
{

}

/**
 * Graphics content manager destructor
 */
GraphicsContentManager::~GraphicsContentManager()
{

}

/**
 * Return a reference to the mesh factory, which constructs new meshes at
 * runtime
 */
MeshFactory& GraphicsContentManager::meshFactory()
{
    return mMeshFactory;
}