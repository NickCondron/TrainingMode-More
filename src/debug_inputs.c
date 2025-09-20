#include "../MexTK/mex.h"

// Constants
#define CIRC_BUFF_SIZE 16

typedef struct {
    bool active;
    DevText *color_dev_text; 
    DevText *lag_dev_text; 
    u8 fetch_index;            // Current circular buffer index
    u32 poll_circ_buff[CIRC_BUFF_SIZE]; // Poll time timestamps
    u32 last_poll_time;        // Last controller poll timestamp
    u32 last_fetch_time;       // Last input fetch timestamp
    u32 latency;               // Total input-to-render latency
    u32 poll_diff_min_us;      // Minimum poll interval
    u32 poll_diff_max_us;      // Maximum poll interval
    u32 fetch_diff_us;         // Time between fetches
    u32 poll_to_fetch_us;      // Poll to fetch time
    u32 poll_to_engine_us;     // Poll to engine processing time
    u32 poll_count;            // Total poll counter
} DebugInput;

DebugInput g_di = { 0 };

// called from controller interrupt handler
void LogPollTime(void) {
    u32 interrupts = OSDisableInterrupts();
    if (!g_di.active)
        return;
    
    // Get current time and calculate difference from last poll
    u32 current_tick = OSGetTick();
    u32 diff_us = OSTicksToUS(current_tick - g_di.last_poll_time);
    g_di.last_poll_time = current_tick;
    
    // Update min/max poll differences (reset every 256 polls)
    if ((g_di.poll_count++ & 0xFF) == 0) {
        // Reset min/max every 256 polls (~2 seconds)
        g_di.poll_diff_min_us = diff_us;
        g_di.poll_diff_max_us = diff_us;
    } else {
        if (diff_us < g_di.poll_diff_min_us) {
            g_di.poll_diff_min_us = diff_us;
        }
        if (diff_us > g_di.poll_diff_max_us) {
            g_di.poll_diff_max_us = diff_us;
        }
    }
    
    OSRestoreInterrupts(interrupts);
}

// called when game reads controller data
void LogFetchTime(HSD_PadData* pads) {
    if (stc_scene_info->major_curr != MJRKIND_VS || stc_scene_info->minor_curr != MNRKIND_MATCH)
        return;

    if (*((s32 *) 0x80479d64) != 0) // believed to be some loading state
        return;

    u32 interrupts = OSDisableInterrupts();
    
    // Store poll time in circular buffer at current fetch index
    g_di.poll_circ_buff[g_di.fetch_index] = g_di.last_poll_time;

    if (!g_di.active)
        g_di.active = true;
    
    // Inject the fetch index into P1's d-pad inputs for tracking
    PADStatus *pad = &pads->stat[0];
    pad->button &= ~(PAD_BUTTON_DPAD_LEFT | PAD_BUTTON_DPAD_RIGHT |
            PAD_BUTTON_DPAD_DOWN | PAD_BUTTON_DPAD_UP);
    pad->button |= g_di.fetch_index;
    
    // Increment fetch index
    g_di.fetch_index = (g_di.fetch_index + 1) % CIRC_BUFF_SIZE;
    
    // Calculate and store timing differences
    u32 current_tick = OSGetTick();
    g_di.fetch_diff_us = OSTicksToUS(current_tick - g_di.last_fetch_time);
    g_di.poll_to_fetch_us = OSTicksToUS(current_tick - g_di.last_poll_time);
    g_di.last_fetch_time = current_tick;
    
    OSRestoreInterrupts(interrupts);
}

// called when game engine processes inputs
void LogEngineTime(void) {
    if (!g_di.active)
        return;
    u32 interrupts = OSDisableInterrupts();
    
    // Extract the key from d-pad inputs and clear them
    HSD_Pad *pad = PadGetMaster(0);
    u8 key = pad->held & 0xF;
    pad->held &= ~0xF; // Clear d-pad to prevent taunting
    
    // Calculate time from fetch to engine processing
    g_di.poll_to_engine_us = USSinceTick(g_di.poll_circ_buff[key]);
    
    u8 grey = key * 16;
    GXColor color = {grey, grey, grey, 255};
    DevelopText_StoreBGColor(g_di.color_dev_text, &color);
    
    OSRestoreInterrupts(interrupts);
}

// Decode yuv color to get index/key
int get_key_from_color(u32 yuv_color) {
    u8 y1 = (yuv_color >> 24) & 0xFF;
    u8 y2 = (yuv_color >> 8) & 0xFF;
    
    if (y1 != y2 || y1 < 15) {
        assert("invalid color");
    }
    
    int temp = (y1 - 15) * 6 / 5;
    return (temp >> 4) & 0xF; // Extract 4 bits for 0-15 range
}

void LogScanoutTime(void) {
    if (stc_scene_info->major_curr != MJRKIND_VS || stc_scene_info->minor_curr != MNRKIND_MATCH)
        return;
    if (*((s32 *) 0x80479d64) != 0) // believed to be some loading state
        return;
    if (!g_di.active)
        return;

    u32 interrupts = OSDisableInterrupts();
    
    void *xfb = (void *) 0x804a8b10;
    u32 color = *(volatile u32*)xfb;

    // invalidate cache
    __asm__ volatile (
        "dcbi 0, %0\n\t"
        "sync\n\t"
        "isync"
        : : "r" (xfb) : "memory"
    );

    if (!color) {
        u8 key = get_key_from_color(color);
        g_di.latency = USSinceTick(g_di.poll_circ_buff[key]);
    }
    
    OSRestoreInterrupts(interrupts);
}


void update_lag_display(void) {
    if (!g_di.active)
        return;

    DevelopText_EraseAllText(g_di.lag_dev_text);
    DevelopText_ResetCursorXY(g_di.lag_dev_text, 0, 0);

    DevelopText_AddString(g_di.lag_dev_text, "Total Game Lag: %u us\n\n", g_di.latency);
    DevelopText_AddString(g_di.lag_dev_text, "Poll Count: %u\n", g_di.poll_count);
    DevelopText_AddString(g_di.lag_dev_text, "Min Poll Diff: %u us\n", g_di.poll_diff_min_us);
    DevelopText_AddString(g_di.lag_dev_text, "Max Poll Diff: %u us\n", g_di.poll_diff_max_us);
    DevelopText_AddString(g_di.lag_dev_text, "Fetch-Fetch: %u us\n", g_di.fetch_diff_us);
    DevelopText_AddString(g_di.lag_dev_text, "Poll-Fetch: %u us\n", g_di.poll_to_fetch_us);
    DevelopText_AddString(g_di.lag_dev_text, "Poll-Engine: %u us\n", g_di.poll_to_engine_us);
}


// Fizzi comment:
// I thought this would fire twice per frame (same as polling),
// but it doesn't and idk what it does
void dummy_polling_handler(void) {
    return;
}

// Initialize the debug input system
bool InitDebugInput(void) {
    // Creates a color square develop text
    DevText *text = DevelopText_CreateDataTable(30, -210, -40, 1, 1, HSD_MemAlloc(32));
    DevelopText_Activate(0, text);
    text->show_cursor = 0;
    GXColor color = {0, 0, 0, 255};
    DevelopText_StoreBGColor(text, &color);
    DevelopText_StoreTextScale(text, 200, 25);
    g_di.color_dev_text = text;

    // Creates lag display develop text
    text = DevelopText_CreateDataTable(31, 0, 0, 29, 9, HSD_MemAlloc(1000));
    DevelopText_Activate(0, text);
    text->show_cursor = 0;
    color = (GXColor) {0, 0, 0, 120};
    DevelopText_StoreBGColor(text, &color);
    color = (GXColor) {220, 220, 220, 255};
    DevelopText_StoreTextColor(text, &color);
    DevelopText_StoreTextScale(text, 10, 17);
    DevelopText_ShowText(text);
    g_di.lag_dev_text = text;

    // Create Proc to update lag display
    GOBJ *gobj = GObj_Create(19, 20, 0);
    GObj_AddProc(gobj, update_lag_display, 7);

    // Register dummy polling handler so that our injections run correctly
    SIRegisterPollingHandler(dummy_polling_handler);

    return true;
}
