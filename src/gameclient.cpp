/*
 * Copyright (C) 2011 Scott MacDonald. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "stdafx.h"
#include "gameclient.h"
#include "common/delete.h"
#include "gui/mainwindow.h"
#include "graphics/dxrenderer.h"

/**
 * Game client constructor
 */
GameClient::GameClient( MainWindow *pMainWindow )
    : mpMainWindow( pMainWindow ),
      mpRenderer( NULL ),
      mKeepRunning( true )
{
    assert( pMainWindow != NULL );
    
}

/**
 * Game client destructor
 */
GameClient::~GameClient()
{
    Delete( mpRenderer );
}

/**
 * Main game loop
 */
App::EProgramStatus GameClient::run()
{
    mpMainWindow->show();

    // Let the game initialize core systems
    if ( initializeEngine() == false || initialize() == false )
    {
        return App::EPROGRAM_FATAL_ERROR;
    }

    // Now load resources before entering the main game loop
    loadContent();

    // The main game loop starts here!
    while ( mKeepRunning && (!mpMainWindow->didUserQuit()) )
    {
        mKeepRunning = mpMainWindow->processMessages();
    }

    // Unload before exiting
    unloadContent();
}

bool GameClient::initializeEngine()
{
    // Create the DirectX renderer
//    mpRenderer = new DXRenderer( mpMainWindow );
//    return mpRenderer->initialize();
    return true;
}

bool GameClient::initialize()
{
    return true;
}

void GameClient::loadContent()
{
    
}

void GameClient::unloadContent()
{

}

void GameClient::update( double currentTime )
{

}

void GameClient::draw( double currentTime )
{

}