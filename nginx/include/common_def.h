#pragma once

#define NGX_SAVE_DELETE(p) { if (p) { delete p; p = nullptr; }}
