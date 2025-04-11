#include "KeyBindHelper.h"

#include <unordered_map>

namespace KeyBinds
{
using KeyCode = KeyBinds::KeyCode;

static const std::unordered_map<UINT, KeyCode> SCAN_CODE_TO_KEY_CODE = {
	{0x01, KeyCode::Escape},
	{0x02, KeyCode::_1},
	{0x03, KeyCode::_2},
	{0x04, KeyCode::_3},
	{0x05, KeyCode::_4},
	{0x06, KeyCode::_5},
	{0x07, KeyCode::_6},
	{0x08, KeyCode::_7},
	{0x09, KeyCode::_8},
	{0x0A, KeyCode::_9},
	{0x0B, KeyCode::_0},
	{0x0C, KeyCode::Minus},
	{0x0D, KeyCode::Equals},
	{0x0E, KeyCode::Backspace},
	{0x0F, KeyCode::Tab},
	{0x10, KeyCode::Q},
	{0x11, KeyCode::W},
	{0x12, KeyCode::E},
	{0x13, KeyCode::R},
	{0x14, KeyCode::T},
	{0x15, KeyCode::Y},
	{0x16, KeyCode::U},
	{0x17, KeyCode::I},
	{0x18, KeyCode::O},
	{0x19, KeyCode::P},
	{0x1A, KeyCode::OpenBracket},
	{0x1B, KeyCode::CloseBracket},
	{0x1C, KeyCode::Enter},
	{0x1D, KeyCode::LeftCtrl},
	{0x1E, KeyCode::A},
	{0x1F, KeyCode::S},
	{0x20, KeyCode::D},
	{0x21, KeyCode::F},
	{0x22, KeyCode::G},
	{0x23, KeyCode::H},
	{0x24, KeyCode::J},
	{0x25, KeyCode::K},
	{0x26, KeyCode::L},
	{0x27, KeyCode::Semicolon},
	{0x28, KeyCode::Quote},
	{0x29, KeyCode::Tilde},
	{0x2A, KeyCode::LeftShift},
	{0x2B, KeyCode::Hash},
	{0x2C, KeyCode::Z},
	{0x2D, KeyCode::X},
	{0x2E, KeyCode::C},
	{0x2F, KeyCode::V},
	{0x30, KeyCode::B},
	{0x31, KeyCode::N},
	{0x32, KeyCode::M},
	{0x33, KeyCode::Colon},
	{0x34, KeyCode::Period},
	{0x35, KeyCode::Slash},
	{0x36, KeyCode::RightShift},
	{0x37, KeyCode::MultiplyNum},
	{0x38, KeyCode::LeftAlt},
	{0x39, KeyCode::Space},
	{0x3A, KeyCode::CapsLock},
	{0x3B, KeyCode::F1},
	{0x3C, KeyCode::F2},
	{0x3D, KeyCode::F3},
	{0x3E, KeyCode::F4},
	{0x3F, KeyCode::F5},
	{0x40, KeyCode::F6},
	{0x41, KeyCode::F7},
	{0x42, KeyCode::F8},
	{0x43, KeyCode::F9},
	{0x44, KeyCode::F10},
	{0x45, KeyCode::Pause}, // not available in GW2!
	{0x46, KeyCode::Scroll}, // not available in GW2!
	{0x47, KeyCode::_7_NUM},
	{0x48, KeyCode::_8_NUM},
	{0x49, KeyCode::_9_NUM},
	{0x4A, KeyCode::MinusNum},
	{0x4B, KeyCode::_4_NUM},
	{0x4C, KeyCode::_5_NUM},
	{0x4D, KeyCode::_6_NUM},
	{0x4E, KeyCode::PlusNum},
	{0x4F, KeyCode::_1_NUM},
	{0x50, KeyCode::_2_NUM},
	{0x51, KeyCode::_3_NUM},
	{0x52, KeyCode::_0_NUM},
	{0x53, KeyCode::DecimalNum},
	{0x56, KeyCode::Backslash},
	{0x57, KeyCode::F11},
	{0x58, KeyCode::F12},
	{0xE01C, KeyCode::EnterNum},
	{0xE01D, KeyCode::RightCtrl},
	{0xE035, KeyCode::DivideNum},
	{0xE037, KeyCode::Print},
	{0xE038, KeyCode::RightAlt},
	{0xE045, KeyCode::NumLock},
	{0xE046, KeyCode::Pause},
	{0xE047, KeyCode::Home},
	{0xE048, KeyCode::ArrowUp},
	{0xE049, KeyCode::Prior},
	{0xE04B, KeyCode::ArrowLeft},
	{0xE04D, KeyCode::ArrowRight},
	{0xE04F, KeyCode::End},
	{0xE050, KeyCode::ArrowDown},
	{0xE051, KeyCode::Next},
	{0xE052, KeyCode::Insert},
	{0xE053, KeyCode::Delete},
	{0xE05B, KeyCode::LeftWin}, // not available in GW2!
	{0xE05C, KeyCode::RightWin}, // not available in GW2!
	{0xE05D, KeyCode::Menu}, // not available in GW2!
};

static const std::unordered_map<KeyCode, UINT> KEY_CODE_TO_SCAN_CODE = {
	{KeyCode::Escape, 0x01},
	{KeyCode::_1, 0x02},
	{KeyCode::_2, 0x03},
	{KeyCode::_3, 0x04},
	{KeyCode::_4, 0x05},
	{KeyCode::_5, 0x06},
	{KeyCode::_6, 0x07},
	{KeyCode::_7, 0x08},
	{KeyCode::_8, 0x09},
	{KeyCode::_9, 0x0A},
	{KeyCode::_0, 0x0B},
	{KeyCode::Minus, 0x0C},
	{KeyCode::Equals, 0x0D},
	{KeyCode::Backspace, 0x0E},
	{KeyCode::Tab, 0x0F},
	{KeyCode::Q, 0x10},
	{KeyCode::W, 0x11},
	{KeyCode::E, 0x12},
	{KeyCode::R, 0x13},
	{KeyCode::T, 0x14},
	{KeyCode::Y, 0x15},
	{KeyCode::U, 0x16},
	{KeyCode::I, 0x17},
	{KeyCode::O, 0x18},
	{KeyCode::P, 0x19},
	{KeyCode::OpenBracket, 0x1A},
	{KeyCode::CloseBracket, 0x1B},
	{KeyCode::Enter, 0x1C},
	{KeyCode::LeftCtrl, 0x1D},
	{KeyCode::A, 0x1E},
	{KeyCode::S, 0x1F},
	{KeyCode::D, 0x20},
	{KeyCode::F, 0x21},
	{KeyCode::G, 0x22},
	{KeyCode::H, 0x23},
	{KeyCode::J, 0x24},
	{KeyCode::K, 0x25},
	{KeyCode::L, 0x26},
	{KeyCode::Semicolon, 0x27},
	{KeyCode::Quote, 0x28},
	{KeyCode::Tilde, 0x29},
	{KeyCode::LeftShift, 0x2A},
	{KeyCode::Hash, 0x2B},
	{KeyCode::Z, 0x2C},
	{KeyCode::X, 0x2D},
	{KeyCode::C, 0x2E},
	{KeyCode::V, 0x2F},
	{KeyCode::B, 0x30},
	{KeyCode::N, 0x31},
	{KeyCode::M, 0x32},
	{KeyCode::Colon, 0x33},
	{KeyCode::Period, 0x34},
	{KeyCode::Slash, 0x35},
	{KeyCode::RightShift, 0x36},
	{KeyCode::MultiplyNum, 0x37},
	{KeyCode::LeftAlt, 0x38},
	{KeyCode::Space, 0x39},
	{KeyCode::CapsLock, 0x3A},
	{KeyCode::F1, 0x3B},
	{KeyCode::F2, 0x3C},
	{KeyCode::F3, 0x3D},
	{KeyCode::F4, 0x3E},
	{KeyCode::F5, 0x3F},
	{KeyCode::F6, 0x40},
	{KeyCode::F7, 0x41},
	{KeyCode::F8, 0x42},
	{KeyCode::F9, 0x43},
	{KeyCode::F10, 0x44},
	{KeyCode::Pause, 0x45}, // not available in GW2!
	{KeyCode::Scroll, 0x46}, // not available in GW2!
	{KeyCode::_7_NUM, 0x47},
	{KeyCode::_8_NUM, 0x48},
	{KeyCode::_9_NUM, 0x49},
	{KeyCode::MinusNum, 0x4A},
	{KeyCode::_4_NUM, 0x4B},
	{KeyCode::_5_NUM, 0x4C},
	{KeyCode::_6_NUM, 0x4D},
	{KeyCode::PlusNum, 0x4E},
	{KeyCode::_1_NUM, 0x4F},
	{KeyCode::_2_NUM, 0x50},
	{KeyCode::_3_NUM, 0x51},
	{KeyCode::_0_NUM, 0x52},
	{KeyCode::DecimalNum, 0x53},
	{KeyCode::Backslash, 0x56},
	{KeyCode::F11, 0x57},
	{KeyCode::F12, 0x58},
	{KeyCode::EnterNum, 0xE01C},
	{KeyCode::RightCtrl, 0xE01D},
	{KeyCode::DivideNum, 0xE035},
	{KeyCode::Print, 0xE037},
	{KeyCode::RightAlt, 0xE038},
	{KeyCode::NumLock, 0xE045},
	{KeyCode::Home, 0xE047},
	{KeyCode::ArrowUp, 0xE048},
	{KeyCode::Prior, 0xE049},
	{KeyCode::ArrowLeft, 0xE04B},
	{KeyCode::ArrowRight, 0xE04D},
	{KeyCode::End, 0xE04F},
	{KeyCode::ArrowDown, 0xE050},
	{KeyCode::Next, 0xE051},
	{KeyCode::Insert, 0xE052},
	{KeyCode::Delete, 0xE053},
	{KeyCode::LeftWin, 0xE05B}, // not available in GW2!
	{KeyCode::RightWin, 0xE05C}, // not available in GW2!
	{KeyCode::Menu, 0xE05D}, // not available in GW2!
};

std::optional<KeyCode> MsvcScanCodeToKeyCode(const UINT pScanCode) {
	/*if (SCAN_CODE_TO_KEY_CODE.contains(pScanCode)) {
		return SCAN_CODE_TO_KEY_CODE.at(pScanCode);
	}*/
	return std::nullopt;
}

UINT KeyCodeToMsvcScanCode(const KeyCode& pScanCode) {
	/*if (KEY_CODE_TO_SCAN_CODE.contains(pScanCode)) {
		return KEY_CODE_TO_SCAN_CODE.at(pScanCode);
	}*/
	return 0;
}
}