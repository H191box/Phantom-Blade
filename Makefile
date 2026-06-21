# ========================================================================
# Phantom Blade - GBA Makefile
# devkitARM build system for 3D Action RPG
# ========================================================================

# Toolchain
export DEVKITARM ?= /opt/devkitpro/devkitARM
export ARM7BIN   := $(DEVKITARM)/bin/arm-none-eabi-
export AS         := $(ARM7BIN)as
export CC         := $(ARM7BIN)gcc
export CXX        := $(ARM7BIN)g++
export LD         := $(ARM7BIN)g++
export OBJCOPY    := $(ARM7BIN)objcopy
export OBJDUMP    := $(ARM7BIN)objdump

# Project name
PROJNAME := phantom_blade

# Output
OUTPUT := $(PROJNAME).gba
ELF    := build/$(PROJNAME).elf
MAP    := build/$(PROJNAME).map

# Directories
SRC_DIR   := src
BUILD_DIR := build
INCLUDES  := include

# ========================================================================
# Source Files
# ========================================================================

# Engine
ENGINE_SRC := \
    src/engine/system.c \
    src/engine/input.c \
    src/engine/dma.c \
    src/engine/timer.c \
    src/engine/math3d.c

# Renderer
RENDER_SRC := \
    src/render/rasterizer.c \
    src/render/rasterizer_gouraud.c \
    src/render/render.c \
    src/render/model.c \
    src/render/camera.c \
    src/render/lighting.c \
    src/render/sprite_overlay.c \
    src/render/minimap.c

# Game Logic
GAME_SRC := \
    src/game/player.c \
    src/game/enemy.c \
    src/game/combat.c \
    src/game/dungeon.c \
    src/game/room_data.c \
    src/game/items.c \
    src/game/dialog.c \
    src/game/game_state.c \
    src/game/hud.c

# Asset Data
ASSET_SRC := \
    src/assets/models_player.c \
    src/assets/models_enemies.c \
    src/assets/models_world.c \
    src/assets/palettes.c \
    src/assets/sprites.c \
    src/assets/font.c \
    src/assets/texture_data.c

# Main
MAIN_SRC := src/main/main.c

# All sources
ALL_SRC := $(ENGINE_SRC) $(RENDER_SRC) $(GAME_SRC) $(ASSET_SRC) $(MAIN_SRC)

# Object files (in build directory)
ALL_OBJ := $(patsubst src/%.c, build/%.o, $(ALL_SRC))

# IWRAM code (hot path - placed in internal WRAM for speed)
IWRAM_SRC := \
    src/engine/math3d.c \
    src/render/rasterizer.c \
    src/render/rasterizer_gouraud.c

# ========================================================================
# Compiler Flags
# ========================================================================

# Architecture: ARM7TDMI with Thumb interworking
ARCH_FLAGS := -mcpu=arm7tdmi -mtune=arm7tdmi -mthumb-interwork

# Optimization: Maximum speed (-O3) for 3D rendering
OPT_FLAGS := -O3 -fomit-frame-pointer

# Unroll inner loops for rasterizer performance
UNROLL_FLAGS := -funroll-loops

# Debug (comment out for release)
# DBG_FLAGS := -g -DDEBUG

# Warning level
WARN_FLAGS := -Wall -Wextra -Wno-unused-parameter

# Include paths
INC_FLAGS := -I$(INCLUDES) -Isrc/engine -Isrc/render -Isrc/game -Isrc/assets

# GBA-specific defines
GBA_FLAGS := -DGBA -DNDEBUG

# Combined C flags
CFLAGS := $(ARCH_FLAGS) $(OPT_FLAGS) $(UNROLL_FLAGS) $(WARN_FLAGS) \
          $(INC_FLAGS) $(GBA_FLAGS) $(DBG_FLAGS)

# Assembly flags
ASFLAGS := $(ARCH_FLAGS) $(INC_FLAGS)

# Linker flags
LDFLAGS := $(ARCH_FLAGS) -Wl,-Map,$(MAP) -Wl,-T,gba.ld \
           -Wl,--gc-sections

# ========================================================================
# Linker Script
# ========================================================================

# We need a linker script for GBA memory layout
# Create one if it doesn't exist
gba.ld:
	@echo "Creating GBA linker script..."
	@echo "OUTPUT_ARCH(arm)" > gba.ld
	@echo "ENTRY(_start)" >> gba.ld
	@echo "MEMORY {" >> gba.ld
	@echo "  rom  (rx) : ORIGIN = 0x08000000, LENGTH = 32M" >> gba.ld
	@echo "  ewram (rw) : ORIGIN = 0x02000000, LENGTH = 256K" >> gba.ld
	@echo "  iwram (rw) : ORIGIN = 0x03000000, LENGTH = 32K" >> gba.ld
	@echo "}" >> gba.ld
	@echo "SECTIONS {" >> gba.ld
	@echo "  .text : {" >> gba.ld
	@echo "    *(.text)" >> gba.ld
	@echo "    *(.text*)" >> gba.ld
	@echo "    *(.rodata)" >> gba.ld
	@echo "    *(.rodata*)" >> gba.ld
	@echo "  } > rom" >> gba.ld
	@echo "  .data : {" >> gba.ld
	@echo "    *(.data)" >> gba.ld
	@echo "    *(.data*)" >> gba.ld
	@echo "  } > ewram AT > rom" >> gba.ld
	@echo "  .bss : {" >> gba.ld
	@echo "    *(.bss)" >> gba.ld
	@echo "    *(.bss*)" >> gba.ld
	@echo "    *(COMMON)" >> gba.ld
	@echo "  } > ewram" >> gba.ld
	@echo "  .iwram (NOLOAD) : {" >> gba.ld
	@echo "    *(.iwram)" >> gba.ld
	@echo "    *(.iwram*)" >> gba.ld
	@echo "  } > iwram" >> gba.ld
	@echo "  /DISCARD/ : {" >> gba.ld
	@echo "    *(.ARM.attributes)" >> gba.ld
	@echo "    *(.comment)" >> gba.ld
	@echo "    *(.note*)" >> gba.ld
	@echo "  }" >> gba.ld
	@echo "}" >> gba.ld

# ========================================================================
# GBA ROM Header (needs to be patched into the binary)
# ========================================================================

# Header patching
HEADER_FIX := header_fix

# ========================================================================
# Build Rules
# ========================================================================

# Default target
.PHONY: all clean

all: gba.ld $(OUTPUT)
	@echo ""
	@echo "=== Phantom Blade Build Complete ==="
	@echo "Output: $(OUTPUT)"
	@echo "Size:   $$(wc -c < $(OUTPUT)) bytes"
	@echo "====================================="

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/engine
	@mkdir -p $(BUILD_DIR)/render
	@mkdir -p $(BUILD_DIR)/game
	@mkdir -p $(BUILD_DIR)/assets
	@mkdir -p $(BUILD_DIR)/main

# Compile C sources to objects
build/%.o: src/%.c | $(BUILD_DIR)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link ELF
$(ELF): $(ALL_OBJ) gba.ld
	@echo "LD $@"
	@$(CC) $(LDFLAGS) -o $@ $(ALL_OBJ) -lgcc

# Convert ELF to GBA ROM
$(OUTPUT): $(ELF)
	@echo "OBJCOPY $@"
	@$(OBJCOPY) -O binary $< $@
	@echo "Fixing GBA header..."
	@python3 -c "
import struct
with open('$@', 'r+b') as f:
    # Read existing data
    f.seek(0)
    data = bytearray(f.read())
    # Ensure at least 192 bytes for header
    while len(data) < 192:
        data.append(0)
    # Entry point at 0x08000000
    struct.pack_into('<I', data, 0, 0x08000000)
    # Nintendo logo (bytes 4-15: fixed logo data)
    logo = bytes([0x24,0xFF,0xAE,0x51,0x69,0x9A,0xA2,0x21,
                 0x3D,0x84,0x82,0x0A,0x84,0xE4,0x09,0xAD,
                 0x11,0x24,0x8B,0x98,0xC0,0x81,0x7F,0x21,
                 0xA3,0x52,0xBE,0x19,0x93,0x09,0xCE,0x20,
                 0x10,0x46,0x4A,0x4A,0xF8,0x27,0x31,0xEC,
                 0x58,0xC7,0xE8,0x33,0x82,0xE3,0xCE,0xBF,
                 0x85,0xF4,0xDF,0x94,0xCE,0x4B,0x09,0xC1,
                 0x94,0x56,0x8A,0xC0,0x13,0x72,0xA7,0xFC,
                 0x9F,0x84,0x4D,0x73,0xA3,0xCA,0x9A,0x61,
                 0x58,0x97,0xA3,0x27,0xFC,0x03,0x98,0x76,
                 0x23,0x1D,0xC7,0x61,0x03,0x04,0xAE,0x56])
    data[4:4+156] = logo
    # Game title at 0xA0 (12 bytes uppercase ASCII)
    title = b'PHANTOMBLADE'
    data[0xA0:0xA0+12] = title
    # Game code at 0xAC (4 bytes)
    data[0xAC:0xAC+4] = b'PBLE'
    # Maker code at 0xB0 (2 bytes)
    data[0xB0:0xB0+2] = b'00'
    # Fixed value at 0xB2
    data[0xB2] = 0x96
    # Main unit code at 0xB3 (0x00 = GBA)
    data[0xB3] = 0x00
    # Device type at 0xB4
    data[0xB4] = 0x00
    # Reserved area 0xB5-0xBB
    for i in range(0xB5, 0xBC):
        data[i] = 0x00
    # Software version at 0xBC
    data[0xBC] = 0x00
    # Complement check (header checksum)
    data[0xBD] = 0x00
    # Reserved 0xBE-0xBF
    data[0xBE] = 0x00
    data[0xBF] = 0x00
    # Write back
    f.seek(0)
    f.write(data)
    # Truncate to original size
    f.truncate(len(data))
"

# ========================================================================
# Clean
# ========================================================================

clean:
	@echo "Cleaning build files..."
	@rm -f $(ALL_OBJ) $(ELF) $(OUTPUT) $(MAP) gba.ld
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete."

# ========================================================================
# Rebuild everything
# ========================================================================

rebuild: clean all
