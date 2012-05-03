/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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


var lastX = 0;
var lastY = 0;
var deltaX = 0;
var deltaY = 0;
var popped = 0;
var matchId = 3;
var leftSwiped = false;
var rightSwiped = false;
var tapped = false;

function reset()
{
lastX = 0;
lastY = 0;
deltaX = 0;
deltaY = 0;
popped = 0;
matchId = 3;
leftSwiped = false;
rightSwiped = false;
tapped = false;
}

function resetTapped()
{
	tapped = false;
}

function isTapped()
{
	return tapped;
}

function Tapped()
{
	tapped = true;
}

function resetSwipe()
{
	leftSwiped = false;
	rightSwiped = false;
}

function swipedLeft()
{
	leftSwiped = true;
}

function isSwipedLeft()
{
	return leftSwiped;
}

function swipedRight()
{
	rightSwiped = true;
}

function isSwipedRight()
{
	return rightSwiped;
}

function startTouch(currentX, currentY)
{
	lastX = currentX;
	lastY = currentY;
	
	return 0;
}

function moveTouch(currentX, currentY)
{
	deltaX += currentX - lastX;
	deltaY += currentY - lastY;
	
	lastX = currentX;
	lastY = currentY;
	
	return 0;
}

function endTouch()
{
	deltaX = 0;
	deltaY = 0;
	
	return 0;
}

function isTap()
{	
	return  Math.abs(deltaX) < 10 && Math.abs(deltaY) < 10;
}

function isSwipeUp()
{
	return deltaY < -300;
}

function isSwipeDown()
{
	return deltaY > 300;
}

function isSwipeLeft()
{
	return deltaX < -300;
}

function isSwipeRight()
{
	return deltaX > 300;
}

function matchUp()
{
	matchId++;
	
	if (matchId == 5)
	{
		return true;
	}
	else if (matchId == 9)
	{
		matchId = 1;
		return true;
	}
	
	return false;
}

function matchDown()
{
	matchId--;
	
	if (matchId == 4)
	{
		return true;
	}
	else if (matchId == 0)
	{
		matchId = 8;
		return true;
	}
	
	return false;
}
