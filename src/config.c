// SPDX-License-Identifier: MIT
// Program configuration.
// Copyright (C) 2022 Artem Senichev <artemsen@gmail.com>

#include "config.h"

#include "array.h"
#include "fs.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Default key/value parameter. */
struct configdef_kv {
    const char* key;
    const char* value;
};

// clang-format off
static const struct configdef_kv def_general[] = {
    { CFG_GNRL_MODE,      "viewer"    },
    { CFG_GNRL_POSITION,  "parent"    },
    { CFG_GNRL_SIZE,      "parent"    },
    { CFG_GNRL_DECOR,     CFG_NO      },
    { CFG_GNRL_SIGUSR1,   "reload"    },
    { CFG_GNRL_SIGUSR2,   "next_file" },
    { CFG_GNRL_WC,        CFG_YES     },
    { CFG_GNRL_APP_ID,    "swayimg"   },
};

static const struct configdef_kv def_viewer[] = {
    { CFG_VIEW_WINDOW,    "#00000000" },
    { CFG_VIEW_TRANSP,    "grid"      },
    { CFG_VIEW_SCALE,     "optimal"   },
    { CFG_VIEW_POSITION,  "center"    },
    { CFG_VIEW_AA,        "mks13"     },
    { CFG_VIEW_SSHOW,     CFG_NO      },
    { CFG_VIEW_SSHOW_TM,  "3"         },
    { CFG_VIEW_HISTORY,   "1"         },
    { CFG_VIEW_PRELOAD,   "1"         },
};

static const struct configdef_kv def_gallery[] = {
    { CFG_GLRY_SIZE,      "200"       },
    { CFG_GLRY_CACHE,     "100"       },
    { CFG_GLRY_PRELOAD,   CFG_NO      },
    { CFG_GLRY_PSTORE,    CFG_NO      },
    { CFG_GLRY_FILL,      CFG_YES     },
    { CFG_GLRY_AA,        "mks13"     },
    { CFG_GLRY_WINDOW,    "#00000000" },
    { CFG_GLRY_BKG,       "#202020ff" },
    { CFG_GLRY_SELECT,    "#404040ff" },
    { CFG_GLRY_BORDER,    "#000000ff" },
    { CFG_GLRY_SHADOW,    "#000000ff" },
};

static const struct configdef_kv def_list[] = {
    { CFG_LIST_FROMFILE,  CFG_NO      },
    { CFG_LIST_ORDER,     "alpha"     },
    { CFG_LIST_REVERSE,   CFG_NO      },
    { CFG_LIST_LOOP,      CFG_YES     },
    { CFG_LIST_RECURSIVE, CFG_NO      },
    { CFG_LIST_ALL,       CFG_NO      },
    { CFG_LIST_FSMON,     CFG_YES     },
};

static const struct configdef_kv def_font[] = {
    { CFG_FONT_NAME,      "monospace" },
    { CFG_FONT_SIZE,      "14"        },
    { CFG_FONT_COLOR,     "#ccccccff" },
    { CFG_FONT_SHADOW,    "#000000d0" },
    { CFG_FONT_BKG,       "#00000000" },
};

static const struct configdef_kv def_info[] = {
    { CFG_INFO_SHOW,      CFG_YES     },
    { CFG_INFO_ITIMEOUT,  "5"         },
    { CFG_INFO_STIMEOUT,  "3"         },
};

static const struct configdef_kv def_infov[] = {
    { CFG_INFO_TL,        "+name,+format,+filesize,+imagesize,+exif" },
    { CFG_INFO_TR,        "index"                                    },
    { CFG_INFO_BL,        "scale,frame"                              },
    { CFG_INFO_BR,        "status"                                   },
};

static const struct configdef_kv def_infog[] = {
    { CFG_INFO_TL,        "none"                                     },
    { CFG_INFO_TR,        "index"                                    },
    { CFG_INFO_BL,        "none"                                     },
    { CFG_INFO_BR,        "name,status"                              },
};

static const struct configdef_kv def_keysv[] = {
    { "F1",               "help"                                     },
    { "Home",             "first_file"                               },
    { "End",              "last_file"                                },
    { "Prior",            "prev_file"                                },
    { "Next",             "next_file"                                },
    { "Space",            "next_file"                                },
    { "Shift+r",          "rand_file"                                },
    { "Shift+d",          "prev_dir"                                 },
    { "d",                "next_dir"                                 },
    { "Shift+o",          "prev_frame"                               },
    { "o",                "next_frame"                               },
    { "c",                "skip_file"                                },
    { "Shift+s",          "slideshow"                                },
    { "s",                "animation"                                },
    { "f",                "fullscreen"                               },
    { "Return",           "mode gallery"                             },
    { "Left",             "step_left 10"                             },
    { "Right",            "step_right 10"                            },
    { "Up",               "step_up 10"                               },
    { "Down",             "step_down 10"                             },
    { "Equal",            "zoom +10"                                 },
    { "Plus",             "zoom +10"                                 },
    { "Minus",            "zoom -10"                                 },
    { "w",                "zoom width"                               },
    { "Shift+w",          "zoom height"                              },
    { "z",                "zoom fit"                                 },
    { "Shift+z",          "zoom fill"                                },
    { "0",                "zoom real"                                },
    { "BackSpace",        "zoom optimal"                             },
    { "Alt+z",            "zoom keep"                                },
    { "Alt+s",            "zoom"                                     },
    { "bracketleft",      "rotate_left"                              },
    { "bracketright",     "rotate_right"                             },
    { "m",                "flip_vertical"                            },
    { "Shift+m",          "flip_horizontal"                          },
    { "a",                "antialiasing next"                        },
    { "Shift+a",          "antialiasing prev"                        },
    { "r",                "reload"                                   },
    { "i",                "info"                                     },
    { "Shift+Delete",     "exec rm '%' && echo \"File removed: %\""  },
    { "Escape",           "exit"                                     },
    { "q",                "exit"                                     },
    { "ScrollLeft",       "step_right 5"                             },
    { "ScrollRight",      "step_left 5"                              },
    { "ScrollUp",         "step_up 5"                                },
    { "ScrollDown",       "step_down 5"                              },
    { "Ctrl+ScrollUp",    "zoom +10"                                 },
    { "Ctrl+ScrollDown",  "zoom -10"                                 },
    { "Shift+ScrollUp",   "prev_file"                                },
    { "Shift+ScrollDown", "next_file"                                },
    { "Alt+ScrollUp",     "prev_frame"                               },
    { "Alt+ScrollDown",   "next_frame"                               },
    { "MouseLeft",        "drag"                                     },
    { "MouseSide",        "prev_file"                                },
    { "MouseExtra",       "next_file"                                },
};

static const struct configdef_kv def_keysg[] = {
    { "F1",               "help"                                     },
    { "Home",             "first_file"                               },
    { "End",              "last_file"                                },
    { "Left",             "step_left"                                },
    { "Right",            "step_right"                               },
    { "Up",               "step_up"                                  },
    { "Down",             "step_down"                                },
    { "Prior",            "page_up"                                  },
    { "Next",             "page_down"                                },
    { "c",                "skip_file"                                },
    { "f",                "fullscreen"                               },
    { "Return",           "mode viewer"                              },
    { "a",                "antialiasing next"                        },
    { "Shift+a",          "antialiasing prev"                        },
    { "r",                "reload"                                   },
    { "i",                "info"                                     },
    { "Equal",            "thumb +20"                                },
    { "Plus",             "thumb +20"                                },
    { "Minus",            "thumb -20"                                },
    { "Shift+Delete",     "exec rm '%' && echo \"File removed: %\""  },
    { "Escape",           "exit"                                     },
    { "q",                "exit"                                     },
    { "ScrollLeft",       "step_right"                               },
    { "ScrollRight",      "step_left"                                },
    { "ScrollUp",         "step_up"                                  },
    { "ScrollDown",       "step_down"                                },
    { "Ctrl+ScrollUp",    "thumb +20"                                },
    { "Ctrl+ScrollDown",  "thumb -20"                                },
    { "MouseLeft",        "mode gallery"                             },
};
// clang-format on

/** Default section. */
struct configdef_section {
    const char* name;
    const struct configdef_kv* params;
    size_t params_num;
};

#define CONFIGDEF_SECTION(n, p) { n, p, sizeof(p) / sizeof(p[0]) }

static const struct configdef_section default_config[] = {
    CONFIGDEF_SECTION(CFG_GENERAL, def_general),
    CONFIGDEF_SECTION(CFG_VIEWER, def_viewer),
    CONFIGDEF_SECTION(CFG_GALLERY, def_gallery),
    CONFIGDEF_SECTION(CFG_LIST, def_list),
    CONFIGDEF_SECTION(CFG_FONT, def_font),
    CONFIGDEF_SECTION(CFG_INFO, def_info),
    CONFIGDEF_SECTION(CFG_INFO_VIEWER, def_infov),
    CONFIGDEF_SECTION(CFG_INFO_GALLERY, def_infog),
    CONFIGDEF_SECTION(CFG_KEYS_VIEWER, def_keysv),
    CONFIGDEF_SECTION(CFG_KEYS_GALLERY, def_keysg),
};

/** Config file location. */
struct location {
    const char* prefix;  ///< Environment variable name
    const char* postfix; ///< Constant postfix
};

static const struct location config_locations[] = {
    { "XDG_CONFIG_HOME", "/swayimg/config"         },
    { "HOME",            "/.config/swayimg/config" },
    { "XDG_CONFIG_DIRS", "/swayimg/config"         },
    { NULL,              "/etc/xdg/swayimg/config" }
};

/**
 * Load configuration from a file.
 * @param cfg config instance
 * @param path full path to the file
 * @return false on errors
 */
static bool load(struct config* cfg, const char* path)
{
    FILE* fd = NULL;
    char* buff = NULL;
    size_t buff_sz = 0;
    size_t line_num = 0;
    ssize_t nread;
    char* section = NULL;

    fd = fopen(path, "r");
    if (!fd) {
        return false;
    }

    while ((nread = getline(&buff, &buff_sz, fd)) != -1) {
        char* delim;
        const char* value;
        char* line = buff;

        ++line_num;

        // trim spaces
        while (nread-- && isspace(line[nread])) {
            line[nread] = 0;
        }
        while (*line && isspace(*line)) {
            ++line;
        }

        // skip empty lines and comments
        if (!*line || *line == '#') {
            continue;
        }

        // check for section beginning
        if (*line == '[') {
            ssize_t len;
            char* new_section;
            ++line;
            delim = strchr(line, ']');
            if (!delim || line + 1 == delim) {
                fprintf(stderr, "WARNING: Invalid config line in %s:%zu\n",
                        path, line_num);
                continue;
            }
            *delim = 0;
            len = delim - line + 1;
            new_section = realloc(section, len);
            if (new_section) {
                section = new_section;
                memcpy(section, line, len);
            }
            continue;
        }

        if (!section) {
            fprintf(stderr,
                    "WARNING: Config parameter without section in %s:%zu\n",
                    path, line_num);
            continue;
        }

        delim = strchr(line, '=');
        if (!delim) {
            fprintf(stderr, "WARNING: Invalid config line in %s:%zu\n", path,
                    line_num);
            continue;
        }

        // trim spaces from start of value
        value = delim + 1;
        while (*value && isspace(*value)) {
            ++value;
        }
        // trim spaces from key
        *delim = 0;
        while (line != delim && isspace(*--delim)) {
            *delim = 0;
        }

        // save configuration parameter
        config_set(cfg, section, line, value);
    }

    free(buff);
    free(section);
    fclose(fd);

    return true;
}

/**
 * Create key/value entry.
 * @param key,value config param
 * @return key/value entry
 */
static struct config_keyval* create_keyval(const char* key, const char* value)
{
    const size_t key_sz = strlen(key);
    struct config_keyval* kv = calloc(1, sizeof(struct config_keyval) + key_sz);
    if (kv) {
        memcpy(kv->key, key, key_sz);
        kv->value = str_dup(value, NULL);
    }
    return kv;
}

/**
 * Create section in the config instance.
 * @param name section name
 * @return section instance
 */
static struct config* create_section(const char* name)
{
    const size_t sz = strlen(name);
    struct config* section = calloc(1, sizeof(struct config) + sz);
    if (section) {
        memcpy(section->name, name, sz);
    }
    return section;
}

/**
 * Convert text value to boolean config.
 * @param text source config value
 * @param value parsed value
 * @param false if text has invalid format
 */
static bool text_to_bool(const char* text, bool* value)
{
    if (strcmp(text, CFG_YES) == 0) {
        *value = true;
    } else if (strcmp(text, CFG_NO) == 0) {
        *value = false;
    } else {
        return false;
    }
    return true;
}

/**
 * Convert text value to color.
 * @param text source config value
 * @param value parsed value
 * @param false if text has invalid format
 */
static bool text_to_color(const char* text, argb_t* value)
{
    char* endptr;
    argb_t color;

    while (*text == '#' || isspace(*text)) {
        ++text;
    }

    errno = 0;

    color = strtoull(text, &endptr, 16);

    if (endptr && !*endptr && errno == 0) {
        if (strlen(text) > 6) { // value with alpha (RRGGBBAA)
            color = (color >> 8) | ARGB_SET_A(color);
        } else {
            color |= ARGB_SET_A(0xff);
        }
        *value = color;
        return true;
    }

    return false;
}

struct config* config_load(void)
{
    struct config* cfg = NULL;

    // load default config
    for (size_t i = 0; i < ARRAY_SIZE(default_config); ++i) {
        const struct configdef_section* ds = &default_config[i];
        struct config* cs = create_section(ds->name);
        if (!cs) {
            continue;
        }
        for (size_t j = 0; j < ds->params_num; ++j) {
            const struct configdef_kv* defkv = &ds->params[j];
            struct config_keyval* kv = create_keyval(defkv->key, defkv->value);
            if (kv) {
                kv->next = cs->params;
                cs->params = kv;
            }
        }
        cs->next = cfg;
        cfg = cs;
    }

    // find and load first available config file
    for (size_t i = 0; i < ARRAY_SIZE(config_locations); ++i) {
        const struct location* cl = &config_locations[i];
        char path[PATH_MAX];
        if (fs_envpath(cl->prefix, cl->postfix, path, sizeof(path)) &&
            load(cfg, path)) {
            break;
        }
    }

    return cfg;
}

void config_free(struct config* cfg)
{
    while (cfg) {
        struct config* next_section = cfg->next;
        while (cfg->params) {
            struct config_keyval* next_kv = cfg->params->next;
            free(cfg->params->value);
            free(cfg->params);
            cfg->params = next_kv;
        }
        free(cfg);
        cfg = next_section;
    }
}

bool config_set(struct config* cfg, const char* section, const char* key,
                const char* value)
{
    struct config_keyval* kv;
    struct config* cs;

    if (!value || !*value) {
        fprintf(stderr,
                "WARNING: Empty config value for key \"%s\" in section \"%s\" "
                "is not allowed\n",
                key, section);
        return false;
    }

    // search for config section
    cs = cfg;
    while (cs && strcmp(section, cs->name) != 0) {
        cs = cs->next;
    }
    if (!cs) {
        fprintf(stderr, "WARNING: Unknown config section \"%s\"\n", section);
        return false;
    }

    // search for existing key/value to replace
    kv = cs->params;
    while (kv) {
        if (strcmp(key, kv->key) == 0) {
            free(kv->value);
            kv->value = str_dup(value, NULL);
            return true;
        }
        kv = kv->next;
    }

    // only key bindings can have their own config keys
    if (strcmp(section, CFG_KEYS_VIEWER) != 0 &&
        strcmp(section, CFG_KEYS_GALLERY) != 0) {
        fprintf(stderr,
                "WARNING: Unknown config key \"%s\" in section \"%s\"\n", key,
                section);
        return false;
    }

    // add new key/value
    kv = create_keyval(key, value);
    if (kv) {
        kv->next = cs->params;
        cs->params = kv;
    }

    return true;
}

bool config_set_arg(struct config* cfg, const char* arg)
{
    char section[32];
    char key[32];
    const char* ptr;
    struct str_slice slices[2];
    size_t size;

    // split section.key and value
    size = str_split(arg, '=', slices, ARRAY_SIZE(slices));
    if (size <= 1) {
        fprintf(stderr, "WARNING: Invalid config argument format: \"%s\"\n",
                arg);
        return false;
    }

    // split section and key
    ptr = slices[0].value + slices[0].len;
    while (*ptr != '.') {
        if (--ptr < arg) {
            fprintf(stderr, "WARNING: Invalid config argument format: \"%s\"\n",
                    arg);
            return false;
        }
    }

    // section name
    size = ptr - slices[0].value;
    if (size > sizeof(section) - 1) {
        size = sizeof(section) - 1;
    }
    memcpy(section, slices[0].value, size);
    section[size] = 0;

    // key name
    ++ptr; // skip dot
    size = slices[0].len - (ptr - slices[0].value);
    if (size > sizeof(key) - 1) {
        size = sizeof(key) - 1;
    }
    memcpy(key, ptr, size);
    key[size] = 0;

    return config_set(cfg, section, key, slices[1].value);
}

const char* config_get_default(const char* section, const char* key)
{
    for (size_t i = 0; i < ARRAY_SIZE(default_config); ++i) {
        const struct configdef_section* ds = &default_config[i];
        if (strcmp(section, ds->name) == 0) {
            for (size_t j = 0; j < ds->params_num; ++j) {
                const struct configdef_kv* kv = &ds->params[j];
                if (strcmp(key, kv->key) == 0) {
                    return kv->value;
                }
            }
        }
    }
    fprintf(stderr,
            "WARNING: Default value for key \"%s\" in section \"%s\" "
            "not found\n",
            key, section);
    return "";
}

const char* config_get(const struct config* cfg, const char* section,
                       const char* key)
{
    const struct config* cs = cfg;
    while (cs && strcmp(section, cs->name) != 0) {
        cs = cs->next;
    }

    if (cs) {
        const struct config_keyval* kv = cs->params;
        while (kv) {
            if (strcmp(key, kv->key) == 0) {
                return kv->value;
            }
            kv = kv->next;
        }
    }

    fprintf(stderr,
            "WARNING: Value for key \"%s\" in section \"%s\" not found\n", key,
            section);
    return "";
}

ssize_t config_get_oneof(const struct config* cfg, const char* section,
                         const char* key, const char** array, size_t array_sz)
{
    const char* value = config_get(cfg, section, key);
    ssize_t index = str_search_index(array, array_sz, value, 0);

    if (index == -1) {
        fprintf(stderr,
                "WARNING: "
                "Invalid config value \"%s = %s\" in section \"%s\": "
                "expected one of: ",
                key, value, section);
        for (size_t i = 0; i < array_sz; ++i) {
            fprintf(stderr, "%s, ", array[i]);
        }
        value = config_get_default(section, key);
        fprintf(stderr, "the default value \"%s\" will be used\n", value);
        index = str_search_index(array, array_sz, value, 0);
    }

    return index >= 0 ? index : 0;
}

bool config_get_bool(const struct config* cfg, const char* section,
                     const char* key)
{
    bool boolean = false;
    const char* value = config_get(cfg, section, key);

    if (!text_to_bool(value, &boolean)) {
        text_to_bool(config_get_default(section, key), &boolean);
        fprintf(stderr,
                "WARNING: "
                "Invalid config value \"%s = %s\" in section \"%s\": "
                "expected \"" CFG_YES "\" or \"" CFG_NO "\", "
                "the default value \"%s\" will be used\n",
                key, value, section, boolean ? CFG_YES : CFG_NO);
    }

    return boolean;
}

ssize_t config_get_num(const struct config* cfg, const char* section,
                       const char* key, ssize_t min_val, ssize_t max_val)
{
    ssize_t num = 0;
    const char* value = config_get(cfg, section, key);

    if (!str_to_num(value, 0, &num, 0) || num < min_val || num > max_val) {
        str_to_num(config_get_default(section, key), 0, &num, 0);
        fprintf(stderr,
                "WARNING: "
                "Invalid config value \"%s = %s\" in section \"%s\": "
                "expected integer in range %zd-%zd, "
                "the default value %zd will be used\n",
                key, value, section, min_val, max_val, num);
    }

    return num;
}

argb_t config_get_color(const struct config* cfg, const char* section,
                        const char* key)
{
    argb_t color = 0;
    const char* value = config_get(cfg, section, key);

    if (!text_to_color(value, &color)) {
        text_to_color(config_get_default(section, key), &color);
        fprintf(stderr,
                "WARNING: "
                "Invalid color value \"%s = %s\" in section \"%s\": "
                "expected RGB(A) format (e.g. #11223344), "
                "the default value #%08x will be used\n",
                key, value, section, color);
    }

    return color;
}

void config_error_key(const char* section, const char* key)
{
    fprintf(stderr, "WARNING: Invalid config key \"%s\" in section \"%s\"\n",
            key, section);
}

void config_error_val(const char* section, const char* value)
{
    fprintf(stderr, "WARNING: Invalid config value \"%s\" in section \"%s\"\n",
            value, section);
}
