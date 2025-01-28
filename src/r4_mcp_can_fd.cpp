/*
 * This software is based on an original work by Longan Labs, licensed under the MIT License.
 * The original source can be found at https://github.com/Longan-Labs/Longan_CANFD.
 *
 * Modifications and enhancements made by OSAKA NDS CO., LTD. in 2025.
 *
 * MIT License
 * 
 * Copyright (c) 2018 Longan Labs
 * Copyright (c) 2025 OSAKA NDS CO., LTD.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "r4_mcp_can_fd.h"

/*********************************************************************************************************
** Function name:           MCP_CAN_1
** Descriptions:            Constructor
*********************************************************************************************************/
MCP_CAN_1::MCP_CAN_1(byte _CS)
{
    pSPI = &SPI;
    init_CS(_CS);
}

/*********************************************************************************************************
** Function name:           init_CS
** Descriptions:            init CS pin and set UNSELECTED
*********************************************************************************************************/
void MCP_CAN_1::init_CS(byte _CS)
{
    if (_CS == 0)
    {
        return;
    }
    SPICS = _CS;
    pinMode(SPICS, OUTPUT);
    digitalWrite(SPICS, HIGH);
}

void MCP_CAN_1::setSPI(SPIClass *_pSPI)
{
    pSPI = _pSPI; // define SPI port to use before begin()
}
