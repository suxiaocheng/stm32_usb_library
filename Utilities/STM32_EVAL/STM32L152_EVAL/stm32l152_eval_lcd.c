/**
  ******************************************************************************
  * @file    stm32l152_eval_lcd.c
  * @author  MCD Application Team
  * @version V5.0.2
  * @date    09-March-2012
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320),
  *          AM-240320LDTNQW00H (LCD_SPFD5408B) and AM240320D5TOQW01H (LCD_ILI9325) 
  *          Liquid Crystal Display Module of STM32L152-EVAL board RevB.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l152_eval_lcd.h"
#include "../Common/fonts.c"

/** @addtogroup Utilities
  * @{
  */

/** @addtogroup STM32_EVAL
  * @{
  */

/** @addtogroup STM32L152_EVAL
  * @{
  */

/** @defgroup STM32L152_EVAL_LCD 
  * @brief   This file includes the LCD driver for AM-240320L8TNQW00H (LCD_ILI9320), 
  *          AM-240320LDTNQW00H (LCD_SPFD5408B) and AM240320D5TOQW01H (LCD_ILI9325) 
  *          Liquid Crystal Display Module of STM32L152-EVAL board.
  * @{
  */

/** @defgroup STM32L152_EVAL_LCD_Private_Types
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32L152_EVAL_LCD_Private_Defines
  * @{
  */
#define LCD_ILI9325        0x9325
#define LCD_ILI9320        0x9320
#define LCD_SPFD5408       0x5408
#define START_BYTE         0x70
#define SET_INDEX          0x00
#define READ_STATUS        0x01
#define LCD_WRITE_REG      0x02
#define LCD_READ_REG       0x03
#define MAX_POLY_CORNERS   200
#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))
/**
  * @}
  */

/** @defgroup STM32L152_EVAL_LCD_Private_Macros
  * @{
  */
#define ABS(X)  ((X) > 0 ? (X) : -(X))
/**
  * @}
  */

/** @defgroup STM32L152_EVAL_LCD_Private_Variables
  * @{
  */
static sFONT *LCD_Currentfonts;
/* Global variables to set the written text color */
static __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;
static __IO uint32_t LCDType = LCD_SPFD5408;
/**
  * @}
  */

/** @defgroup STM32L152_EVAL_LCD_Private_Function_Prototypes
  * @{
  */
#ifndef USE_Delay
static void delay(__IO uint32_t nCount);
#endif /* USE_Delay */

static void PutPixel(int16_t x, int16_t y);
static void LCD_PolyLineRelativeClosed(pPoint Points, uint16_t PointCount,
				       uint16_t Closed);

/**
  * @}
  */

/** @defgroup STM32L152_EVAL_LCD_Private_Functions
  * @{
  */

/**
  * @brief  DeInitializes the LCD.
  * @param  None
  * @retval None
  */
void STM32L152_LCD_DeInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*!< LCD Display Off */
	LCD_DisplayOff();

	/*!< LCD_SPI disable */
	SPI_Cmd(LCD_SPI, DISABLE);

	/*!< LCD_SPI DeInit */
	SPI_DeInit(LCD_SPI);

	/*!< Disable SPI clock  */
	RCC_APB1PeriphClockCmd(LCD_SPI_CLK, DISABLE);

	/* Configure NCS in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LCD_NCS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LCD_NCS_GPIO_PORT, &GPIO_InitStructure);

	/* Configure SPI pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = LCD_SPI_SCK_PIN;
	GPIO_Init(LCD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_SPI_MISO_PIN;
	GPIO_Init(LCD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LCD_SPI_MOSI_PIN;
	GPIO_Init(LCD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Setups the LCD.
  * @param  None
  * @retval None
  */
void LCD_Setup(void)
{
/* Configure the LCD Control pins --------------------------------------------*/
	LCD_CtrlLinesConfig();

/* Configure the LCD_SPI interface ----------------------------------------------*/
	LCD_SPIConfig();

	if (LCDType == LCD_SPFD5408) {
		/* Start Initial Sequence -------------------------------------------------- */
		LCD_WriteReg(LCD_REG_227, 0x3008);	/* Set internal timing */
		LCD_WriteReg(LCD_REG_231, 0x0012);	/* Set internal timing */
		LCD_WriteReg(LCD_REG_239, 0x1231);	/* Set internal timing */
		LCD_WriteReg(LCD_REG_1, 0x0100);	/* Set SS and SM bit */
		LCD_WriteReg(LCD_REG_2, 0x0700);	/* Set 1 line inversion */
		LCD_WriteReg(LCD_REG_3, 0x1030);	/* Set GRAM write direction and BGR=1. */
		LCD_WriteReg(LCD_REG_4, 0x0000);	/* Resize register */
		LCD_WriteReg(LCD_REG_8, 0x0202);	/* Set the back porch and front porch */
		LCD_WriteReg(LCD_REG_9, 0x0000);	/* Set non-display area refresh cycle ISC[3:0] */
		LCD_WriteReg(LCD_REG_10, 0x0000);	/* FMARK function */
		LCD_WriteReg(LCD_REG_12, 0x0000);	/* RGB interface setting */
		LCD_WriteReg(LCD_REG_13, 0x0000);	/* Frame marker Position */
		LCD_WriteReg(LCD_REG_15, 0x0000);	/* RGB interface polarity */
		/* Power On sequence ------------------------------------------------------- */
		LCD_WriteReg(LCD_REG_16, 0x0000);	/* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(LCD_REG_17, 0x0000);	/* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(LCD_REG_18, 0x0000);	/* VREG1OUT voltage */
		LCD_WriteReg(LCD_REG_19, 0x0000);	/* VDV[4:0] for VCOM amplitude */
		_delay_(20);	/* Dis-charge capacitor power voltage (200ms) */
		LCD_WriteReg(LCD_REG_17, 0x0007);	/* DC1[2:0], DC0[2:0], VC[2:0] */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_16, 0x12B0);	/* SAP, BT[3:0], AP, DSTB, SLP, STB */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_18, 0x01BD);	/* External reference voltage= Vci */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_19, 0x1400);	/* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(LCD_REG_41, 0x000E);	/* VCM[4:0] for VCOMH */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_32, 0x0000);	/* GRAM horizontal Address */
		LCD_WriteReg(LCD_REG_33, 0x013F);	/* GRAM Vertical Address */
		/* Adjust the Gamma Curve -------------------------------------------------- */
		LCD_WriteReg(LCD_REG_48, 0x0007);
		LCD_WriteReg(LCD_REG_49, 0x0302);
		LCD_WriteReg(LCD_REG_50, 0x0105);
		LCD_WriteReg(LCD_REG_53, 0x0206);
		LCD_WriteReg(LCD_REG_54, 0x0808);
		LCD_WriteReg(LCD_REG_55, 0x0206);
		LCD_WriteReg(LCD_REG_56, 0x0504);
		LCD_WriteReg(LCD_REG_57, 0x0007);
		LCD_WriteReg(LCD_REG_60, 0x0105);
		LCD_WriteReg(LCD_REG_61, 0x0808);
		/* Set GRAM area ----------------------------------------------------------- */
		LCD_WriteReg(LCD_REG_80, 0x0000);	/* Horizontal GRAM Start Address */
		LCD_WriteReg(LCD_REG_81, 0x00EF);	/* Horizontal GRAM End Address */
		LCD_WriteReg(LCD_REG_82, 0x0000);	/* Vertical GRAM Start Address */
		LCD_WriteReg(LCD_REG_83, 0x013F);	/* Vertical GRAM End Address */
		LCD_WriteReg(LCD_REG_96, 0xA700);	/* Gate Scan Line */
		LCD_WriteReg(LCD_REG_97, 0x0001);	/* NDL,VLE, REV */
		LCD_WriteReg(LCD_REG_106, 0x0000);	/* Set scrolling line */
		/* Partial Display Control ------------------------------------------------- */
		LCD_WriteReg(LCD_REG_128, 0x0000);
		LCD_WriteReg(LCD_REG_129, 0x0000);
		LCD_WriteReg(LCD_REG_130, 0x0000);
		LCD_WriteReg(LCD_REG_131, 0x0000);
		LCD_WriteReg(LCD_REG_132, 0x0000);
		LCD_WriteReg(LCD_REG_133, 0x0000);
		/* Panel Control ----------------------------------------------------------- */
		LCD_WriteReg(LCD_REG_144, 0x0010);
		LCD_WriteReg(LCD_REG_146, 0x0000);
		LCD_WriteReg(LCD_REG_147, 0x0003);
		LCD_WriteReg(LCD_REG_149, 0x0110);
		LCD_WriteReg(LCD_REG_151, 0x0000);
		LCD_WriteReg(LCD_REG_152, 0x0000);
		/* Set GRAM write direction and BGR = 1
		   I/D=01 (Horizontal : increment, Vertical : decrement)
		   AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(LCD_REG_3, 0x1018);
		LCD_WriteReg(LCD_REG_7, 0x0112);	/* 262K color and display ON */
	} else if (LCDType == LCD_ILI9320) {
		_delay_(5);	/* Delay 50 ms */
		/* Start Initial Sequence ------------------------------------------------ */
		LCD_WriteReg(LCD_REG_229, 0x8000);	/* Set the internal vcore voltage */
		LCD_WriteReg(LCD_REG_0, 0x0001);	/* Start internal OSC. */
		LCD_WriteReg(LCD_REG_1, 0x0100);	/* set SS and SM bit */
		LCD_WriteReg(LCD_REG_2, 0x0700);	/* set 1 line inversion */
		LCD_WriteReg(LCD_REG_3, 0x1030);	/* set GRAM write direction and BGR=1. */
		LCD_WriteReg(LCD_REG_4, 0x0000);	/* Resize register */
		LCD_WriteReg(LCD_REG_8, 0x0202);	/* set the back porch and front porch */
		LCD_WriteReg(LCD_REG_9, 0x0000);	/* set non-display area refresh cycle ISC[3:0] */
		LCD_WriteReg(LCD_REG_10, 0x0000);	/* FMARK function */
		LCD_WriteReg(LCD_REG_12, 0x0000);	/* RGB interface setting */
		LCD_WriteReg(LCD_REG_13, 0x0000);	/* Frame marker Position */
		LCD_WriteReg(LCD_REG_15, 0x0000);	/* RGB interface polarity */
		/* Power On sequence ----------------------------------------------------- */
		LCD_WriteReg(LCD_REG_16, 0x0000);	/* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(LCD_REG_17, 0x0000);	/* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(LCD_REG_18, 0x0000);	/* VREG1OUT voltage */
		LCD_WriteReg(LCD_REG_19, 0x0000);	/* VDV[4:0] for VCOM amplitude */
		_delay_(20);	/* Dis-charge capacitor power voltage (200ms) */
		LCD_WriteReg(LCD_REG_16, 0x17B0);	/* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(LCD_REG_17, 0x0137);	/* DC1[2:0], DC0[2:0], VC[2:0] */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_18, 0x0139);	/* VREG1OUT voltage */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_19, 0x1d00);	/* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(LCD_REG_41, 0x0013);	/* VCM[4:0] for VCOMH */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_32, 0x0000);	/* GRAM horizontal Address */
		LCD_WriteReg(LCD_REG_33, 0x0000);	/* GRAM Vertical Address */
		/* Adjust the Gamma Curve ------------------------------------------------ */
		LCD_WriteReg(LCD_REG_48, 0x0006);
		LCD_WriteReg(LCD_REG_49, 0x0101);
		LCD_WriteReg(LCD_REG_50, 0x0003);
		LCD_WriteReg(LCD_REG_53, 0x0106);
		LCD_WriteReg(LCD_REG_54, 0x0b02);
		LCD_WriteReg(LCD_REG_55, 0x0302);
		LCD_WriteReg(LCD_REG_56, 0x0707);
		LCD_WriteReg(LCD_REG_57, 0x0007);
		LCD_WriteReg(LCD_REG_60, 0x0600);
		LCD_WriteReg(LCD_REG_61, 0x020b);

		/* Set GRAM area --------------------------------------------------------- */
		LCD_WriteReg(LCD_REG_80, 0x0000);	/* Horizontal GRAM Start Address */
		LCD_WriteReg(LCD_REG_81, 0x00EF);	/* Horizontal GRAM End Address */
		LCD_WriteReg(LCD_REG_82, 0x0000);	/* Vertical GRAM Start Address */
		LCD_WriteReg(LCD_REG_83, 0x013F);	/* Vertical GRAM End Address */
		LCD_WriteReg(LCD_REG_96, 0x2700);	/* Gate Scan Line */
		LCD_WriteReg(LCD_REG_97, 0x0001);	/* NDL,VLE, REV */
		LCD_WriteReg(LCD_REG_106, 0x0000);	/* set scrolling line */
		/* Partial Display Control ----------------------------------------------- */
		LCD_WriteReg(LCD_REG_128, 0x0000);
		LCD_WriteReg(LCD_REG_129, 0x0000);
		LCD_WriteReg(LCD_REG_130, 0x0000);
		LCD_WriteReg(LCD_REG_131, 0x0000);
		LCD_WriteReg(LCD_REG_132, 0x0000);
		LCD_WriteReg(LCD_REG_133, 0x0000);
		/* Panel Control --------------------------------------------------------- */
		LCD_WriteReg(LCD_REG_144, 0x0010);
		LCD_WriteReg(LCD_REG_146, 0x0000);
		LCD_WriteReg(LCD_REG_147, 0x0003);
		LCD_WriteReg(LCD_REG_149, 0x0110);
		LCD_WriteReg(LCD_REG_151, 0x0000);
		LCD_WriteReg(LCD_REG_152, 0x0000);
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=01 (Horizontal : increment, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(LCD_REG_3, 0x1018);
		LCD_WriteReg(LCD_REG_7, 0x0173);	/* 262K color and display ON */
	} else if (LCDType == LCD_ILI9325) {
		/* Start Initial Sequence ------------------------------------------------ */
		LCD_WriteReg(LCD_REG_0, 0x0001);	/* Start internal OSC. */
		LCD_WriteReg(LCD_REG_1, 0x0100);	/* Set SS and SM bit */
		LCD_WriteReg(LCD_REG_2, 0x0700);	/* Set 1 line inversion */
		LCD_WriteReg(LCD_REG_3, 0x1018);	/* Set GRAM write direction and BGR=1. */
		LCD_WriteReg(LCD_REG_4, 0x0000);	/* Resize register */
		LCD_WriteReg(LCD_REG_8, 0x0202);	/* Set the back porch and front porch */
		LCD_WriteReg(LCD_REG_9, 0x0000);	/* Set non-display area refresh cycle ISC[3:0] */
		LCD_WriteReg(LCD_REG_10, 0x0000);	/* FMARK function */
		LCD_WriteReg(LCD_REG_12, 0x0000);	/* RGB interface setting */
		LCD_WriteReg(LCD_REG_13, 0x0000);	/* Frame marker Position */
		LCD_WriteReg(LCD_REG_15, 0x0000);	/* RGB interface polarity */

		/* Power On sequence ----------------------------------------------------- */
		LCD_WriteReg(LCD_REG_16, 0x0000);	/* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(LCD_REG_17, 0x0000);	/* DC1[2:0], DC0[2:0], VC[2:0] */
		LCD_WriteReg(LCD_REG_18, 0x0000);	/* VREG1OUT voltage */
		LCD_WriteReg(LCD_REG_19, 0x0000);	/* VDV[4:0] for VCOM amplitude */
		_delay_(20);	/* Dis-charge capacitor power voltage (200ms) */
		LCD_WriteReg(LCD_REG_16, 0x17B0);	/* SAP, BT[3:0], AP, DSTB, SLP, STB */
		LCD_WriteReg(LCD_REG_17, 0x0137);	/* DC1[2:0], DC0[2:0], VC[2:0] */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_18, 0x0139);	/* VREG1OUT voltage */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_19, 0x1d00);	/* VDV[4:0] for VCOM amplitude */
		LCD_WriteReg(LCD_REG_41, 0x0013);	/* VCM[4:0] for VCOMH */
		_delay_(5);	/* Delay 50 ms */
		LCD_WriteReg(LCD_REG_32, 0x0000);	/* GRAM horizontal Address */
		LCD_WriteReg(LCD_REG_33, 0x0000);	/* GRAM Vertical Address */

		/* Adjust the Gamma Curve (ILI9325)--------------------------------------- */
		LCD_WriteReg(LCD_REG_48, 0x0007);
		LCD_WriteReg(LCD_REG_49, 0x0302);
		LCD_WriteReg(LCD_REG_50, 0x0105);
		LCD_WriteReg(LCD_REG_53, 0x0206);
		LCD_WriteReg(LCD_REG_54, 0x0808);
		LCD_WriteReg(LCD_REG_55, 0x0206);
		LCD_WriteReg(LCD_REG_56, 0x0504);
		LCD_WriteReg(LCD_REG_57, 0x0007);
		LCD_WriteReg(LCD_REG_60, 0x0105);
		LCD_WriteReg(LCD_REG_61, 0x0808);

		/* Set GRAM area --------------------------------------------------------- */
		LCD_WriteReg(LCD_REG_80, 0x0000);	/* Horizontal GRAM Start Address */
		LCD_WriteReg(LCD_REG_81, 0x00EF);	/* Horizontal GRAM End Address */
		LCD_WriteReg(LCD_REG_82, 0x0000);	/* Vertical GRAM Start Address */
		LCD_WriteReg(LCD_REG_83, 0x013F);	/* Vertical GRAM End Address */

		LCD_WriteReg(LCD_REG_96, 0xA700);	/* Gate Scan Line(GS=1, scan direction is G320~G1) */
		LCD_WriteReg(LCD_REG_97, 0x0001);	/* NDL,VLE, REV */
		LCD_WriteReg(LCD_REG_106, 0x0000);	/* set scrolling line */

		/* Partial Display Control ----------------------------------------------- */
		LCD_WriteReg(LCD_REG_128, 0x0000);
		LCD_WriteReg(LCD_REG_129, 0x0000);
		LCD_WriteReg(LCD_REG_130, 0x0000);
		LCD_WriteReg(LCD_REG_131, 0x0000);
		LCD_WriteReg(LCD_REG_132, 0x0000);
		LCD_WriteReg(LCD_REG_133, 0x0000);

		/* Panel Control --------------------------------------------------------- */
		LCD_WriteReg(LCD_REG_144, 0x0010);
		LCD_WriteReg(LCD_REG_146, 0x0000);
		LCD_WriteReg(LCD_REG_147, 0x0003);
		LCD_WriteReg(LCD_REG_149, 0x0110);
		LCD_WriteReg(LCD_REG_151, 0x0000);
		LCD_WriteReg(LCD_REG_152, 0x0000);

		/* set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : increment, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(LCD_REG_3, 0x1018);

		LCD_WriteReg(LCD_REG_7, 0x0133);	/* 262K color and display ON */
	}
}

/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
void STM32L152_LCD_Init(void)
{
	__IO uint32_t lcdid = 0;

	/* Setups the LCD */
	LCD_Setup();

	/* Read the LCD ID */
	lcdid = LCD_ReadReg(0x00);

	if (lcdid == LCD_SPFD5408) {
		LCDType = LCD_SPFD5408;
	} else if (lcdid == LCD_ILI9320) {
		LCDType = LCD_ILI9320;
		/* Setups the LCD */
		LCD_Setup();
	} else {
		LCDType = LCD_ILI9325;
		/* Setups the LCD */
		LCD_Setup();
	}

	LCD_SetFont(&LCD_DEFAULT_FONT);
}

/**
  * @brief  Sets the LCD Text and Background colors.
  * @param  _TextColor: specifies the Text Color.
  * @param  _BackColor: specifies the Background Color.
  * @retval None
  */
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
	TextColor = _TextColor;
	BackColor = _BackColor;
}

/**
  * @brief  Gets the LCD Text and Background colors.
  * @param  _TextColor: pointer to the variable that will contain the Text 
            Color.
  * @param  _BackColor: pointer to the variable that will contain the Background 
            Color.
  * @retval None
  */
void LCD_GetColors(__IO uint16_t * _TextColor, __IO uint16_t * _BackColor)
{
	*_TextColor = TextColor;
	*_BackColor = BackColor;
}

/**
  * @brief  Sets the Text color.
  * @param  Color: specifies the Text color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetTextColor(__IO uint16_t Color)
{
	TextColor = Color;
}

/**
  * @brief  Sets the Background color.
  * @param  Color: specifies the Background color code RGB(5-6-5).
  * @retval None
  */
void LCD_SetBackColor(__IO uint16_t Color)
{
	BackColor = Color;
}

/**
  * @brief  Sets the Text Font.
  * @param  fonts: specifies the font to be used.
  * @retval None
  */
void LCD_SetFont(sFONT * fonts)
{
	LCD_Currentfonts = fonts;
}

/**
  * @brief  Gets the Text Font.
  * @param  None.
  * @retval the used font.
  */
sFONT *LCD_GetFont(void)
{
	return LCD_Currentfonts;
}

/**
  * @brief  Clears the selected line.
  * @param  Line: the Line to be cleared.
  *         This parameter can be one of the following values:
  *             @arg Linex: where x can be 0..n
  * @retval None
  */
void LCD_ClearLine(uint16_t Line)
{
	uint16_t refcolumn = LCD_PIXEL_WIDTH - 1;

	/* Send the string character by character on lCD */
	while (((refcolumn + 1) & 0xFFFF) >= LCD_Currentfonts->Width) {
		/* Display one character on LCD */
		LCD_DisplayChar(Line, refcolumn, ' ');
		/* Decrement the column position by 16 */
		refcolumn -= LCD_Currentfonts->Width;
	}
}

/**
  * @brief  Clears the hole LCD.
  * @param  Color: the color of the background.
  * @retval None
  */
void LCD_Clear(uint16_t Color)
{
	uint32_t index = 0;

	LCD_SetCursor(0x00, 0x013F);

	LCD_WriteRAM_Prepare();	/* Prepare to write GRAM */

	for (index = 0; index < 76800; index++) {
		LCD_WriteRAM(Color);
	}

	LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);

}

/**
  * @brief  Sets the cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position. 
  * @retval None
  */
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
	LCD_WriteReg(LCD_REG_32, Xpos);
	LCD_WriteReg(LCD_REG_33, Ypos);
}

/**
  * @brief  Draws a character on LCD.
  * @param  Xpos: the Line where to display the character shape.
  * @param  Ypos: start column address.
  * @param  c: pointer to the character data.
  * @retval None
  */
void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t * c)
{
	uint32_t index = 0, i = 0;
	uint16_t Xaddress = 0;

	Xaddress = Xpos;

	LCD_SetCursor(Xaddress, Ypos);

	for (index = 0; index < LCD_Currentfonts->Height; index++) {
		LCD_WriteRAM_Prepare();	/* Prepare to write GRAM */

		for (i = 0; i < LCD_Currentfonts->Width; i++) {
			if ((((c[index] &
			       ((0x80 << ((LCD_Currentfonts->Width / 12) * 8))
				>> i)) == 0x00)
			     && (LCD_Currentfonts->Width <= 12))
			    || (((c[index] & (0x1 << i)) == 0x00)
				&& (LCD_Currentfonts->Width > 12)))
			{
				LCD_WriteRAM(BackColor);
			} else {
				LCD_WriteRAM(TextColor);
			}
		}

		LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);
		Xaddress++;
		LCD_SetCursor(Xaddress, Ypos);
	}
}

/**
  * @brief  Displays one character (16dots width, 24dots height).
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  Column: start column address.
  * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
  * @retval None
  */
void LCD_DisplayChar(uint16_t Line, uint16_t Column, uint8_t Ascii)
{
	Ascii -= 32;
	LCD_DrawChar(Line, Column,
		     &LCD_Currentfonts->table[Ascii *
					      LCD_Currentfonts->Height]);
}

/**
  * @brief  Displays a maximum of 20 char on the LCD.
  * @param  Line: the Line where to display the character shape .
  *   This parameter can be one of the following values:
  *     @arg Linex: where x can be 0..9
  * @param  *ptr: pointer to string to display on LCD.
  * @retval None
  */
void LCD_DisplayStringLine(uint16_t Line, uint8_t * ptr)
{
	uint16_t refcolumn = LCD_PIXEL_WIDTH - 1;

	/* Send the string character by character on lCD */
	while ((*ptr != 0) & (((refcolumn + 1) & 0xFFFF) >=
			      LCD_Currentfonts->Width)) {
		/* Display one character on LCD */
		LCD_DisplayChar(Line, refcolumn, *ptr);
		/* Decrement the column position by 16 */
		refcolumn -= LCD_Currentfonts->Width;
		/* Point on the next character */
		ptr++;
	}
}

/**
  * @brief  Sets a display window
  * @param  Xpos: specifies the X buttom left position.
  * @param  Ypos: specifies the Y buttom left position.
  * @param  Height: display window height.
  * @param  Width: display window width.
  * @retval None
  */
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint8_t Height,
			  uint16_t Width)
{
	/* Horizontal GRAM Start Address */
	if (Xpos >= Height) {
		LCD_WriteReg(LCD_REG_80, (Xpos - Height + 1));
	} else {
		LCD_WriteReg(LCD_REG_80, 0);
	}
	/* Horizontal GRAM End Address */
	LCD_WriteReg(LCD_REG_81, Xpos);
	/* Vertical GRAM Start Address */
	if (Ypos >= Width) {
		LCD_WriteReg(LCD_REG_82, (Ypos - Width + 1));
	} else {
		LCD_WriteReg(LCD_REG_82, 0);
	}
	/* Vertical GRAM End Address */
	LCD_WriteReg(LCD_REG_83, Ypos);

	LCD_SetCursor(Xpos, Ypos);
}

/**
  * @brief  Disables LCD Window mode.
  * @param  None
  * @retval None
  */
void LCD_WindowModeDisable(void)
{
	LCD_SetDisplayWindow(239, 0x13F, 240, 320);
	LCD_WriteReg(LCD_REG_3, 0x1018);
}

/**
  * @brief  Displays a line.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Length: line length.
  * @param  Direction: line direction.
  *         This parameter can be one of the following values: Vertical or Horizontal.
  * @retval None
  */
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length,
		  uint8_t Direction)
{
	uint32_t i = 0;

	LCD_SetCursor(Xpos, Ypos);

	if (Direction == LCD_DIR_HORIZONTAL) {
		LCD_WriteRAM_Prepare();	/* Prepare to write GRAM */

		for (i = 0; i < Length; i++) {
			LCD_WriteRAM(TextColor);
		}
		LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);
	} else {
		for (i = 0; i < Length; i++) {
			LCD_WriteRAMWord(TextColor);
			Xpos++;
			LCD_SetCursor(Xpos, Ypos);
		}
	}
}

/**
  * @brief  Displays a rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: display rectangle height.
  * @param  Width: display rectangle width.
  * @retval None
  */
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint8_t Height, uint16_t Width)
{
	LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
	LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);

	LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
	LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);
}

/**
  * @brief  Displays a circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int32_t D;		/* Decision Variable */
	uint32_t CurX;		/* Current X Value */
	uint32_t CurY;		/* Current Y Value */

	D = 3 - (Radius << 1);
	CurX = 0;
	CurY = Radius;

	while (CurX <= CurY) {
		LCD_SetCursor(Xpos + CurX, Ypos + CurY);
		LCD_WriteRAMWord(TextColor);
		LCD_SetCursor(Xpos + CurX, Ypos - CurY);
		LCD_WriteRAMWord(TextColor);

		LCD_SetCursor(Xpos - CurX, Ypos + CurY);
		LCD_WriteRAMWord(TextColor);

		LCD_SetCursor(Xpos - CurX, Ypos - CurY);
		LCD_WriteRAMWord(TextColor);

		LCD_SetCursor(Xpos + CurY, Ypos + CurX);
		LCD_WriteRAMWord(TextColor);

		LCD_SetCursor(Xpos + CurY, Ypos - CurX);
		LCD_WriteRAMWord(TextColor);

		LCD_SetCursor(Xpos - CurY, Ypos + CurX);
		LCD_WriteRAMWord(TextColor);

		LCD_SetCursor(Xpos - CurY, Ypos - CurX);
		LCD_WriteRAMWord(TextColor);

		if (D < 0) {
			D += (CurX << 2) + 6;
		} else {
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

/**
  * @brief  Displays a monocolor picture.
  * @param  Pict: pointer to the picture array.
  * @retval None
  */
void LCD_DrawMonoPict(const uint32_t * Pict)
{
	uint32_t index = 0, i = 0;
	LCD_SetCursor(0, (LCD_PIXEL_WIDTH - 1));

	LCD_WriteRAM_Prepare();	/* Prepare to write GRAM */

	for (index = 0; index < 2400; index++) {
		for (i = 0; i < 32; i++) {
			if ((Pict[index] & (1 << i)) == 0x00) {
				LCD_WriteRAM(BackColor);
			} else {
				LCD_WriteRAM(TextColor);
			}
		}
	}

	LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);
}

#ifdef USE_LCD_DrawBMP
/**
  * @brief  Displays a bitmap picture loaded in the SPI Flash.
  * @param  BmpAddress: Bmp picture address in the SPI Flash.
  * @retval None
  */
void LCD_DrawBMP(uint32_t BmpAddress)
{
	uint32_t i = 0, size = 0;
	/* Read bitmap size */
	sFLASH_ReadBuffer((uint8_t *) & size, BmpAddress + 2, 4);
	/* get bitmap data address offset */
	sFLASH_ReadBuffer((uint8_t *) & i, BmpAddress + 10, 4);

	size = (size - i) / 2;
	sFLASH_StartReadSequence(BmpAddress + i);
	/* Disable LCD_SPI  */
	SPI_Cmd(LCD_SPI, DISABLE);
	/* SPI in 16-bit mode */
	SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_16b);
	/* Enable LCD_SPI  */
	SPI_Cmd(LCD_SPI, ENABLE);

	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		/* Set GRAM write direction and BGR = 1 */
		/* I/D=00 (Horizontal : decrement, Vertical : decrement) */
		/* AM=1 (address is updated in vertical writing direction) */
		LCD_WriteReg(LCD_REG_3, 0x1008);
		LCD_WriteRAM_Prepare();	/* Prepare to write GRAM */
	}

	/* Read bitmap data from SPI Flash and send them to LCD */
	for (i = 0; i < size; i++) {
		LCD_WriteRAM(__REV16(sFLASH_SendHalfWord(0xA5A5)));
	}
	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		LCD_CtrlLinesWrite(LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET);
	}

	/* Deselect the FLASH: Chip Select high */
	sFLASH_CS_HIGH();
	/* Disable LCD_SPI  */
	SPI_Cmd(LCD_SPI, DISABLE);
	/* SPI in 8-bit mode */
	SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_8b);
	/* Enable LCD_SPI  */
	SPI_Cmd(LCD_SPI, ENABLE);

	if ((LCDType == LCD_ILI9320) || (LCDType == LCD_SPFD5408)) {
		/* Set GRAM write direction and BGR = 1 */
		/* I/D = 01 (Horizontal : increment, Vertical : decrement) */
		/* AM = 1 (address is updated in vertical writing direction) */
		LCD_WriteReg(LCD_REG_3, 0x1018);
	}
}
#endif /* USE_LCD_DrawBMP */

/**
  * @brief  Displays a full rectangle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Height: rectangle height.
  * @param  Width: rectangle width.
  * @retval None
  */
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
		      uint16_t Height)
{
	LCD_SetTextColor(TextColor);

	LCD_DrawLine(Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
	LCD_DrawLine((Xpos + Height), Ypos, Width, LCD_DIR_HORIZONTAL);

	LCD_DrawLine(Xpos, Ypos, Height, LCD_DIR_VERTICAL);
	LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, LCD_DIR_VERTICAL);

	Width -= 2;
	Height--;
	Ypos--;

	LCD_SetTextColor(BackColor);

	while (Height--) {
		LCD_DrawLine(++Xpos, Ypos, Width, LCD_DIR_HORIZONTAL);
	}

	LCD_SetTextColor(TextColor);
}

/**
  * @brief  Displays a full circle.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  Radius
  * @retval None
  */
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
	int32_t D;		/* Decision Variable */
	uint32_t CurX;		/* Current X Value */
	uint32_t CurY;		/* Current Y Value */

	D = 3 - (Radius << 1);

	CurX = 0;
	CurY = Radius;

	LCD_SetTextColor(BackColor);

	while (CurX <= CurY) {
		if (CurY > 0) {
			LCD_DrawLine(Xpos - CurX, Ypos + CurY, 2 * CurY,
				     LCD_DIR_HORIZONTAL);
			LCD_DrawLine(Xpos + CurX, Ypos + CurY, 2 * CurY,
				     LCD_DIR_HORIZONTAL);
		}

		if (CurX > 0) {
			LCD_DrawLine(Xpos - CurY, Ypos + CurX, 2 * CurX,
				     LCD_DIR_HORIZONTA