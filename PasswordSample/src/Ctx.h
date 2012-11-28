/*
 * Copyright © 1996-2012 Certicom Corp. All rights reserved.
 * 
 * This software contains trade secrets, confidential information, and other
 * intellectual property of Certicom Corp. and its licensors. This software
 * cannot be used, reproduced, or distributed in whole or in part by any means
 * without the explicit prior consent of Certicom Corp. Such consent must arise
 * from a separate license agreement from Certicom or its licensees, as 
 * appropriate.
 * 
 * Certicom, Certicom AMS, ACC, Asset Control Core, Certicom Bar Code
 * Authentication Agent, Certicom ECC Core, Certicom Security Architecture,
 * Certicom Trusted Infrastructure, Certicom CodeSign, Certicom KeyInject,
 * ChipActivate, DieMax, Security Builder, Security Builder API, 
 * Security Builder API for .NET, Security Builder BSP, Security Builder Crypto,
 * Security Builder ETS, Security Builder GSE, Security Builder IPSec, Security
 * Builder MCE, Security Builder NSE, Security Builder PKI, Security Builder SSL
 * and SysActivate are trademarks or registered trademarks of Certicom Corp.
 * All other companies and products listed herein are trademarks or registered
 * trademarks of their respective holders.
 *
 * BlackBerry®, RIM®, Research In Motion® and related trademarks are owned by
 * Research In Motion Limited. Used under license.
 * 
 */
/* ------------------------------------------------------------------------
 *  "Security Builder" is a registered trademark of Certicom Corp.
 * ------------------------------------------------------------------------
 *
 *  Ctx.h -- Prototypes for functions in Ctx.c
 *
 */ 

#ifndef CTX_H
#define CTX_H

#include "sbdef.h"

int
CreateSbContexts(sb_RNGCtx * rng, sb_YieldCtx * yield, sb_GlobalCtx* sbCtx);

void
DestroySbContexts(sb_RNGCtx * rng, sb_YieldCtx * yield, sb_GlobalCtx* sbCtx);

#endif

