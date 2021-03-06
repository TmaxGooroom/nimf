/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 2; tab-width: 2 -*- */
/*
 * nimf-anthy.c
 * This file is part of Nimf.
 *
 * Copyright (C) 2016-2020 Hodong Kim <cogniti@gmail.com>
 *
 * Nimf is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Nimf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program;  If not, see <http://www.gnu.org/licenses/>.
 */

#include <nimf.h>
#include <anthy/anthy.h>
#include <glib/gi18n.h>

#define NIMF_TYPE_ANTHY             (nimf_anthy_get_type ())
#define NIMF_ANTHY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), NIMF_TYPE_ANTHY, NimfAnthy))
#define NIMF_ANTHY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), NIMF_TYPE_ANTHY, NimfAnthyClass))
#define NIMF_IS_ANTHY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), NIMF_TYPE_ANTHY))
#define NIMF_IS_ANTHY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), NIMF_TYPE_ANTHY))
#define NIMF_ANTHY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), NIMF_TYPE_ANTHY, NimfAnthyClass))

#define NIMF_ANTHY_BUFFER_SIZE  256

typedef struct _NimfAnthy      NimfAnthy;
typedef struct _NimfAnthyClass NimfAnthyClass;

struct _NimfAnthy
{
  NimfEngine parent_instance;

  NimfCandidatable  *candidatable;
  GString           *preedit;
  gint               preedit_offset; /* in bytes */
  gint               preedit_dx;     /* in bytes */
  NimfPreeditState   preedit_state;
  NimfPreeditAttr  **preedit_attrs;
  gchar             *id;
  GSettings         *settings;
  NimfKey          **hiragana_keys;
  NimfKey          **katakana_keys;
  gchar             *method;
  gboolean           method_changed;
  gint               n_input_mode;

  anthy_context_t  context;
  gint             current_segment;
  gchar            buffer[NIMF_ANTHY_BUFFER_SIZE];
  gint             current_page;
  gint             n_pages;
  gint            *selections;
};

struct _NimfAnthyClass
{
  /*< private >*/
  NimfEngineClass parent_class;
};

typedef enum
{
  COMMON,
  EXPLICIT
} NInputMode;

static gint        nimf_anthy_ref_count = 0;
static GHashTable *nimf_anthy_romaji = NULL;;

typedef struct
{
  gchar *key;
  gchar *value;
} RomajiMap;

static const RomajiMap romaji_map[] = {
  {"a", "???"},
  {"b", ""}, /* dummy */
  {"ba", "???"},
  {"be", "???"},
  {"bi", "???"},
  {"bo", "???"},
  {"bu", "???"},
  {"by", ""}, /* dummy */
  {"bya", "??????"},
  {"bye", "??????"},
  {"byi", "??????"},
  {"byo", "??????"},
  {"byu", "??????"},
  {"c", ""}, /* dummy */
  {"ca", "???"},
  {"ce", "???"},
  {"ch", ""}, /* dummy */
  {"cha", "??????"},
  {"che", "??????"},
  {"chi", "???"},
  {"cho", "??????"},
  {"chu", "??????"},
  {"ci", "???"},
  {"co", "???"},
  {"cu", "???"},
  {"cy", ""}, /* dummy */
  {"cya", "??????"},
  {"cye", "??????"},
  {"cyi", "??????"},
  {"cyo", "??????"},
  {"cyu", "??????"},
  {"d", ""}, /* dummy */
  {"da", "???"},
  {"de", "???"},
  {"dh", ""}, /* dummy */
  {"dha", "??????"},
  {"dhe", "??????"},
  {"dhi", "??????"},
  {"dho", "??????"},
  {"dhu", "??????"},
  {"di", "???"},
  {"do", "???"},
  {"du", "???"},
  {"dw", ""}, /* dummy */
  {"dwa", "??????"},
  {"dwe", "??????"},
  {"dwi", "??????"},
  {"dwo", "??????"},
  {"dwu", "??????"},
  {"dy", ""}, /* dummy */
  {"dya", "??????"},
  {"dye", "??????"},
  {"dyi", "??????"},
  {"dyo", "??????"},
  {"dyu", "??????"},
  {"e", "???"},
  {"f", ""}, /* dummy */
  {"fa", "??????"},
  {"fe", "??????"},
  {"fi", "??????"},
  {"fo", "??????"},
  {"fu", "???"},
  {"fw", ""}, /* dummy */
  {"fwa", "??????"},
  {"fwe", "??????"},
  {"fwi", "??????"},
  {"fwo", "??????"},
  {"fwu", "??????"},
  {"fy", ""}, /* dummy */
  {"fya", "??????"},
  {"fye", "??????"},
  {"fyi", "??????"},
  {"fyo", "??????"},
  {"fyu", "??????"},
  {"g", ""}, /* dummy */
  {"ga", "???"},
  {"ge", "???"},
  {"gi", "???"},
  {"go", "???"},
  {"gu", "???"},
  {"gw", ""}, /* dummy */
  {"gwa", "??????"},
  {"gwe", "??????"},
  {"gwi", "??????"},
  {"gwo", "??????"},
  {"gwu", "??????"},
  {"gy", ""}, /* dummy */
  {"gya", "??????"},
  {"gye", "??????"},
  {"gyi", "??????"},
  {"gyo", "??????"},
  {"gyu", "??????"},
  {"h", ""}, /* dummy */
  {"ha", "???"},
  {"he", "???"},
  {"hi", "???"},
  {"ho", "???"},
  {"hu", "???"},
  {"hy", ""}, /* dummy */
  {"hya", "??????"},
  {"hye", "??????"},
  {"hyi", "??????"},
  {"hyo", "??????"},
  {"hyu", "??????"},
  {"i", "???"},
  {"j", ""}, /* dummy */
  {"ja", "??????"},
  {"je", "??????"},
  {"ji", "???"},
  {"jo", "??????"},
  {"ju", "??????"},
  {"jy", ""}, /* dummy */
  {"jya", "??????"},
  {"jye", "??????"},
  {"jyi", "??????"},
  {"jyo", "??????"},
  {"jyu", "??????"},
  {"k", ""}, /* dummy */
  {"ka", "???"},
  {"ke", "???"},
  {"ki", "???"},
  {"ko", "???"},
  {"ku", "???"},
  {"kw", ""}, /* dummy */
  {"kwa", "??????"},
  {"ky", ""}, /* dummy */
  {"kya", "??????"},
  {"kye", "??????"},
  {"kyi", "??????"},
  {"kyo", "??????"},
  {"kyu", "??????"},
  {"l", ""}, /* dummy */
  {"la", "???"},
  {"le", "???"},
  {"li", "???"},
  {"lk", ""}, /* dummy */
  {"lka", "???"},
  {"lke", "???"},
  {"lo", "???"},
  {"lt", ""}, /* dummy */
  {"lts", ""}, /* dummy */
  {"ltsu", "???"},
  {"ltu", "???"},
  {"lu", "???"},
  {"lw", ""}, /* dummy */
  {"lwa", "???"},
  {"ly", ""}, /* dummy */
  {"lya", "???"},
  {"lye", "???"},
  {"lyi", "???"},
  {"lyo", "???"},
  {"lyu", "???"},
  {"m", ""}, /* dummy */
  {"ma", "???"},
  {"me", "???"},
  {"mi", "???"},
  {"mo", "???"},
  {"mu", "???"},
  {"my", ""}, /* dummy */
  {"mya", "??????"},
  {"mye", "??????"},
  {"myi", "??????"},
  {"myo", "??????"},
  {"myu", "??????"},
  {"n", ""}, /* dummy */
  {"na", "???"},
  {"ne", "???"},
  {"ni", "???"},
  {"nn", "???"},
  {"no", "???"},
  {"nu", "???"},
  {"ny", ""}, /* dummy */
  {"nya", "??????"},
  {"nye", "??????"},
  {"nyi", "??????"},
  {"nyo", "??????"},
  {"nyu", "??????"},
  {"o", "???"},
  {"p", ""}, /* dummy */
  {"pa", "???"},
  {"pe", "???"},
  {"pi", "???"},
  {"po", "???"},
  {"pu", "???"},
  {"py", ""}, /* dummy */
  {"pya", "??????"},
  {"pye", "??????"},
  {"pyi", "??????"},
  {"pyo", "??????"},
  {"pyu", "??????"},
  {"q", ""}, /* dummy */
  {"qa", "??????"},
  {"qe", "??????"},
  {"qi", "??????"},
  {"qo", "??????"},
  {"qu", "???"},
  {"qw", ""}, /* dummy */
  {"qwa", "??????"},
  {"qwe", "??????"},
  {"qwi", "??????"},
  {"qwo", "??????"},
  {"qwu", "??????"},
  {"qy", ""}, /* dummy */
  {"qya", "??????"},
  {"qye", "??????"},
  {"qyi", "??????"},
  {"qyo", "??????"},
  {"qyu", "??????"},
  {"r", ""}, /* dummy */
  {"ra", "???"},
  {"re", "???"},
  {"ri", "???"},
  {"ro", "???"},
  {"ru", "???"},
  {"ry", ""}, /* dummy */
  {"rya", "??????"},
  {"rye", "??????"},
  {"ryi", "??????"},
  {"ryo", "??????"},
  {"ryu", "??????"},
  {"s", ""}, /* dummy */
  {"sa", "???"},
  {"se", "???"},
  {"sh", ""}, /* dummy */
  {"sha", "??????"},
  {"she", "??????"},
  {"shi", "???"},
  {"sho", "??????"},
  {"shu", "??????"},
  {"si", "???"},
  {"so", "???"},
  {"su", "???"},
  {"sw", ""}, /* dummy */
  {"swa", "??????"},
  {"swe", "??????"},
  {"swi", "??????"},
  {"swo", "??????"},
  {"swu", "??????"},
  {"sy", ""}, /* dummy */
  {"sya", "??????"},
  {"sye", "??????"},
  {"syi", "??????"},
  {"syo", "??????"},
  {"syu", "??????"},
  {"t", ""}, /* dummy */
  {"ta", "???"},
  {"te", "???"},
  {"th", ""}, /* dummy */
  {"tha", "??????"},
  {"the", "??????"},
  {"thi", "??????"},
  {"tho", "??????"},
  {"thu", "??????"},
  {"ti", "???"},
  {"to", "???"},
  {"ts", ""}, /* dummy */
  {"tsa", "??????"},
  {"tse", "??????"},
  {"tsi", "??????"},
  {"tso", "??????"},
  {"tsu", "???"},
  {"tu", "???"},
  {"tw", ""}, /* dummy */
  {"twa", "??????"},
  {"twe", "??????"},
  {"twi", "??????"},
  {"two", "??????"},
  {"twu", "??????"},
  {"ty", ""}, /* dummy */
  {"tya", "??????"},
  {"tye", "??????"},
  {"tyi", "??????"},
  {"tyo", "??????"},
  {"tyu", "??????"},
  {"u", "???"},
  {"v", ""}, /* dummy */
  {"va", "??????"},
  {"ve", "??????"},
  {"vi", "??????"},
  {"vo", "??????"},
  {"vu", "???"},
  {"vy", ""}, /* dummy */
  {"vya", "??????"},
  {"vye", "??????"},
  {"vyi", "??????"},
  {"vyo", "??????"},
  {"vyu", "??????"},
  {"w", ""}, /* dummy */
  {"wa", "???"},
  {"we", "??????"},
  {"wh", ""}, /* dummy */
  {"wha", "??????"},
  {"whe", "??????"},
  {"whi", "??????"},
  {"who", "??????"},
  {"whu", "???"},
  {"wi", "??????"},
  {"wo", "???"},
  {"wu", "???"},
  {"wy", ""}, /* dummy */
  {"wye", "???"},
  {"wyi", "???"},
  {"x", ""}, /* dummy */
  {"xa", "???"},
  {"xe", "???"},
  {"xi", "???"},
  {"xk", ""}, /* dummy */
  {"xka", "???"},
  {"xke", "???"},
  {"xn", "???"},
  {"xo", "???"},
  {"xt", ""}, /* dummy */
  {"xts", ""}, /* dummy */
  {"xtsu", "???"},
  {"xtu", "???"},
  {"xu", "???"},
  {"xw", ""}, /* dummy */
  {"xwa", "???"},
  {"xy", ""}, /* dummy */
  {"xya", "???"},
  {"xye", "???"},
  {"xyi", "???"},
  {"xyo", "???"},
  {"xyu", "???"},
  {"y", ""}, /* dummy */
  {"ya", "???"},
  {"ye", "??????"},
  {"yi", "???"},
  {"yo", "???"},
  {"yu", "???"},
  {"z", ""}, /* dummy */
  {"za", "???"},
  {"ze", "???"},
  {"zi", "???"},
  {"zo", "???"},
  {"zu", "???"},
  {"zy", ""}, /* dummy */
  {"zya", "??????"},
  {"zye", "??????"},
  {"zyi", "??????"},
  {"zyo", "??????"},
  {"zyu", "??????"},
  {",", "???"},
  {".", "???"},
  {"-", "???"}
};

G_DEFINE_DYNAMIC_TYPE (NimfAnthy, nimf_anthy, NIMF_TYPE_ENGINE);

static void
nimf_anthy_update_preedit_state (NimfEngine    *engine,
                                 NimfServiceIC *target,
                                 const gchar   *new_preedit,
                                 gint           cursor_pos)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  if (anthy->preedit_state == NIMF_PREEDIT_STATE_END &&
      anthy->preedit->len > 0)
  {
    anthy->preedit_state = NIMF_PREEDIT_STATE_START;
    nimf_engine_emit_preedit_start (engine, target);
  }

  nimf_engine_emit_preedit_changed (engine, target, new_preedit,
                                    anthy->preedit_attrs, cursor_pos);

  if (!nimf_service_ic_get_use_preedit (target))
    nimf_candidatable_set_auxiliary_text (anthy->candidatable,
                                          anthy->preedit->str,
                                          g_utf8_strlen (anthy->preedit->str,
                                                         anthy->preedit_offset + anthy->preedit_dx));

  if (anthy->preedit_state == NIMF_PREEDIT_STATE_START &&
      anthy->preedit->len == 0)
  {
    anthy->preedit_state = NIMF_PREEDIT_STATE_END;
    nimf_engine_emit_preedit_end (engine, target);
  }
}

static void
nimf_anthy_emit_commit (NimfEngine    *engine,
                        NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);
  struct anthy_conv_stat conv_stat;
  gint i;

  anthy_get_stat (anthy->context, &conv_stat);

  for (i = 0; i < conv_stat.nr_segment; i++)
    anthy_commit_segment (anthy->context, i, anthy->selections[i]);

  if (anthy->preedit->len > 0)
  {
    nimf_engine_emit_commit (engine, target, anthy->preedit->str);
    g_string_assign (anthy->preedit,  "");
    anthy->preedit_offset = 0;
    anthy->preedit_dx     = 0;
    anthy->preedit_attrs[0]->start_index = 0;
    anthy->preedit_attrs[0]->end_index   = 0;
    anthy->preedit_attrs[1]->start_index = 0;
    anthy->preedit_attrs[1]->end_index   = 0;
    nimf_anthy_update_preedit_state (engine, target, "", 0);
  }
}

void
nimf_anthy_reset (NimfEngine    *engine,
                  NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);
  struct anthy_conv_stat conv_stat;

  anthy_get_stat (anthy->context, &conv_stat);
  nimf_candidatable_hide (anthy->candidatable);
  nimf_anthy_emit_commit (engine, target);
  memset (anthy->selections, 0, conv_stat.nr_segment * sizeof (gint));
  anthy_reset_context (anthy->context);
}

void
nimf_anthy_focus_in (NimfEngine    *engine,
                     NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);
}

void
nimf_anthy_focus_out (NimfEngine    *engine,
                      NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  nimf_candidatable_hide (NIMF_ANTHY (engine)->candidatable);
  nimf_anthy_reset (engine, target);
}

static gint
nimf_anthy_get_current_page (NimfEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  return NIMF_ANTHY (engine)->current_page;
}

static void
nimf_anthy_convert_preedit_text (NimfEngine    *engine,
                                 NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);
  struct anthy_conv_stat conv_stat;
  gint i;
  gint offset  = 0;
  gint end_pos = 0;
  gint len     = 0;
  gint current_segment_len = 0;

  g_string_assign (anthy->preedit, "");
  anthy_get_stat (anthy->context, &conv_stat);

  for (i = 0; i < conv_stat.nr_segment; i++)
  {
    anthy_get_segment (anthy->context, i, anthy->selections[i], anthy->buffer, NIMF_ANTHY_BUFFER_SIZE);
    len = g_utf8_strlen (anthy->buffer, -1);
    end_pos += len;

    if (i < anthy->current_segment)
      offset += len;

    if (i == anthy->current_segment)
      current_segment_len = len;

    g_string_append (anthy->preedit, anthy->buffer);
  }

  anthy->preedit_attrs[0]->start_index = 0;
  anthy->preedit_attrs[0]->end_index = end_pos;
  anthy->preedit_attrs[1]->start_index = offset;
  anthy->preedit_attrs[1]->end_index = offset + current_segment_len;
  nimf_anthy_update_preedit_state (engine, target, anthy->preedit->str,
                                   g_utf8_strlen (anthy->preedit->str, -1));

  anthy->preedit_offset = anthy->preedit->len;
  anthy->preedit_dx     = 0;
}

static void
nimf_anthy_update_preedit_text (NimfEngine    *engine,
                                NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  anthy->preedit_attrs[0]->start_index = 0;
  anthy->preedit_attrs[0]->end_index   = g_utf8_strlen (anthy->preedit->str, -1);
  anthy->preedit_attrs[1]->start_index = 0;
  anthy->preedit_attrs[1]->end_index   = 0;
  nimf_anthy_update_preedit_state (engine, target, anthy->preedit->str,
    g_utf8_strlen (anthy->preedit->str,
                   anthy->preedit_offset + anthy->preedit_dx));
}

static void
nimf_anthy_update_page (NimfEngine    *engine,
                        NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);
  struct anthy_conv_stat    conv_stat;
  struct anthy_segment_stat segment_stat;
  gint i;

  anthy_get_stat (anthy->context, &conv_stat);
  anthy_get_segment_stat (anthy->context, anthy->current_segment, &segment_stat);

  anthy->n_pages = (segment_stat.nr_candidate + 9) / 10;
  nimf_candidatable_clear (anthy->candidatable, target);

  for (i = (anthy->current_page - 1) * 10;
       i < MIN (anthy->current_page * 10, segment_stat.nr_candidate);
       i++)
  {
    anthy_get_segment (anthy->context, anthy->current_segment, i,
                       anthy->buffer, NIMF_ANTHY_BUFFER_SIZE);
    nimf_candidatable_append (anthy->candidatable, anthy->buffer, NULL);
  }

  nimf_candidatable_select_item_by_index_in_page
    (anthy->candidatable, anthy->selections[anthy->current_segment]);
  nimf_candidatable_set_page_values (anthy->candidatable, target,
                                     anthy->current_page, anthy->n_pages, 10);
}

static void
on_candidate_clicked (NimfEngine    *engine,
                      NimfServiceIC *target,
                      gchar         *text,
                      gint           index)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  anthy->selections[anthy->current_segment] = (anthy->current_page -1) * 10 + index;
  nimf_anthy_convert_preedit_text (engine, target);
}

static void
nimf_anthy_page_end (NimfEngine *engine, NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  if (anthy->current_page == anthy->n_pages)
  {
    nimf_candidatable_select_last_item_in_page (anthy->candidatable);
    return;
  }

  anthy->current_page = anthy->n_pages;
  nimf_anthy_update_page (engine, target);
  nimf_candidatable_select_last_item_in_page (anthy->candidatable);
}

static gboolean
nimf_anthy_page_up (NimfEngine *engine, NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  if (anthy->current_page <= 1)
  {
    nimf_anthy_page_end (engine, target);
    return FALSE;
  }

  anthy->current_page--;
  nimf_anthy_update_page (engine, target);
  nimf_candidatable_select_last_item_in_page (anthy->candidatable);

  return TRUE;
}

static void
nimf_anthy_page_home (NimfEngine *engine, NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  if (anthy->current_page <= 1)
  {
    nimf_candidatable_select_first_item_in_page (anthy->candidatable);
    return;
  }

  anthy->current_page = 1;
  nimf_anthy_update_page (engine, target);
  nimf_candidatable_select_first_item_in_page (anthy->candidatable);
}

static gboolean
nimf_anthy_page_down (NimfEngine *engine, NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  if (anthy->current_page == anthy->n_pages)
  {
    nimf_anthy_page_home (engine, target);
    return FALSE;
  }

  anthy->current_page++;
  nimf_anthy_update_page (engine, target);
  nimf_candidatable_select_first_item_in_page (anthy->candidatable);

  return TRUE;
}

static void
on_candidate_scrolled (NimfEngine    *engine,
                       NimfServiceIC *target,
                       gdouble        value)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  if ((gint) value == nimf_anthy_get_current_page (engine))
    return;

  while (anthy->n_pages > 1)
  {
    gint d = (gint) value - nimf_anthy_get_current_page (engine);

    if (d > 0)
      nimf_anthy_page_down (engine, target);
    else if (d < 0)
      nimf_anthy_page_up (engine, target);
    else if (d == 0)
      break;
  }
}

static void
nimf_anthy_update_candidate (NimfEngine    *engine,
                             NimfServiceIC *target)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);
  struct anthy_conv_stat conv_stat;

  anthy_get_stat (anthy->context, &conv_stat);

  if (conv_stat.nr_segment > 0)
  {
    anthy->current_page = 1;
    nimf_anthy_update_page (engine, target);
  }
  else
  {
    if (anthy->n_pages > 0)
    {
      nimf_candidatable_hide (anthy->candidatable);
      nimf_candidatable_clear (anthy->candidatable, target);
      anthy->n_pages = 0;
      anthy->current_page = 0;
    }
  }
}

static gboolean
nimf_anthy_filter_event_romaji (NimfEngine    *engine,
                                NimfServiceIC *target,
                                NimfEvent     *event)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy   *anthy = NIMF_ANTHY (engine);
  const gchar *str;

  g_string_insert_c (anthy->preedit, anthy->preedit_offset + anthy->preedit_dx, event->key.keyval);
  anthy->preedit_dx++;

  while (TRUE)
  {
    gchar *key;

    key = g_strndup (anthy->preedit->str + anthy->preedit_offset,
                     anthy->preedit_dx);
    str = g_hash_table_lookup (nimf_anthy_romaji, key);

    g_free (key);

    if (str)
    {
      if (str[0] != 0)
      {
        g_string_erase (anthy->preedit, anthy->preedit_offset,
                        anthy->preedit_dx);
        g_string_insert (anthy->preedit, anthy->preedit_offset, str);
        anthy->preedit_offset += strlen (str);
        anthy->preedit_dx = 0;
      }

      break;
    }
    else
    {
      if (anthy->preedit_dx > 1)
      {
        char a = anthy->preedit->str[anthy->preedit_offset + anthy->preedit_dx - 2];
        char b = anthy->preedit->str[anthy->preedit_offset + anthy->preedit_dx - 1];

        if (a != 'n' && a == b)
        {
          g_string_erase  (anthy->preedit, anthy->preedit_offset + anthy->preedit_dx - 2, 1);
          g_string_insert (anthy->preedit, anthy->preedit_offset + anthy->preedit_dx - 2, "???");
          anthy->preedit_offset += strlen ("???") - 1;
        }
        else if (anthy->n_input_mode == COMMON && a == 'n' &&
                 !(b == 'a' || b == 'e' || b == 'i' || b == 'o' || b == 'u' || b == 'n'))
        {
          g_string_erase  (anthy->preedit, anthy->preedit_offset + anthy->preedit_dx - 2, 1);
          g_string_insert (anthy->preedit, anthy->preedit_offset + anthy->preedit_dx - 2, "???");
          anthy->preedit_offset += strlen ("???") - 1;
        }

        anthy->preedit_offset += anthy->preedit_dx - 1;
        anthy->preedit_dx = 1;
      }
      else
      {
        break;
      }
    }
  }

  return TRUE;
}

static void
nimf_anthy_preedit_insert (NimfAnthy   *anthy,
                           const gchar *val)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_string_insert (anthy->preedit, anthy->preedit_offset, val);
  anthy->preedit_offset += strlen (val);
}

static gboolean
nimf_anthy_preedit_offset_has_suffix (NimfAnthy *anthy, const gchar *suffix)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gint len = strlen (suffix);

  return !!g_strstr_len (anthy->preedit->str + anthy->preedit_offset - len,
                         len, suffix);
}

static gboolean
nimf_anthy_filter_event_pc104 (NimfEngine    *engine,
                               NimfServiceIC *target,
                               NimfEvent     *event)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);

  gboolean is_shift = event->key.state & NIMF_SHIFT_MASK;

  switch (event->key.hardware_keycode)
  {
    case 49: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 10: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 11: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 12:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 13:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 14:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 15:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 16:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 17:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 18:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 19:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 20:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 21: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 24: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 25: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 26:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 27: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 28: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 29: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 30: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 31: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 32: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 33: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 34:
      if (is_shift)
      {
        nimf_anthy_preedit_insert (anthy, "???");
      }
      else
      {
        if (anthy->preedit->len == 0)
          break;

        if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
      }
      break;
    case 35:
      if (is_shift)
      {
        nimf_anthy_preedit_insert (anthy, "???");
      }
      else
      {
        if (anthy->preedit->len == 0)
          break;

        if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
        else if (nimf_anthy_preedit_offset_has_suffix (anthy, "???"))
          g_string_overwrite (anthy->preedit,
                              anthy->preedit_offset - strlen ("???"), "???");
      }
      break;
    case 51: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 38: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 39: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 40: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 41: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 42: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 43: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 44: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 45: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 46: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 47: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 48: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 52:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 53: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 54: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 55: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 56: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 57: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 58: nimf_anthy_preedit_insert (anthy, "???"); break;
    case 59:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 60:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    case 61:
      if (is_shift) nimf_anthy_preedit_insert (anthy, "???");
      else          nimf_anthy_preedit_insert (anthy, "???");
      break;
    default:
      return FALSE;
  }

  return TRUE;
}

static gchar *
nimf_anthy_convert_to (NimfAnthy *anthy, int candidate_type)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  struct   anthy_conv_stat conv_stat;
  GString *string;
  gint     i;

  anthy_set_string (anthy->context, anthy->preedit->str);
  anthy_get_stat (anthy->context, &conv_stat);
  string = g_string_new ("");
  memset (anthy->buffer, 0, NIMF_ANTHY_BUFFER_SIZE);

  for (i = 0; i < conv_stat.nr_segment; i++)
  {
    anthy_get_segment (anthy->context, i, candidate_type,
                       anthy->buffer, NIMF_ANTHY_BUFFER_SIZE);
    g_string_append (string, anthy->buffer);
  }

  anthy->preedit_offset = anthy->preedit->len;
  anthy->preedit_dx     = 0;

  return g_string_free (string, FALSE);
}

static void
nimf_anthy_replace_last_n (NimfAnthy *anthy)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  if (g_str_has_suffix (anthy->preedit->str, "n"))
  {
    g_string_erase  (anthy->preedit, anthy->preedit->len - 1, 1);
    g_string_append (anthy->preedit, "???");
  }
}

static gboolean
nimf_anthy_filter_event (NimfEngine    *engine,
                         NimfServiceIC *target,
                         NimfEvent     *event)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (engine);
  gboolean   retval;

  if (event->key.type == NIMF_EVENT_KEY_RELEASE)
    return FALSE;

  if (anthy->method_changed)
  {
    nimf_anthy_reset (engine, target);
    anthy->method_changed = FALSE;
  }

  if (nimf_candidatable_is_visible (anthy->candidatable))
  {
    switch (event->key.keyval)
    {
      case NIMF_KEY_Return:
      case NIMF_KEY_KP_Enter:
        nimf_candidatable_hide (anthy->candidatable);
        nimf_anthy_emit_commit (engine, target);
        return TRUE;
      case NIMF_KEY_Up:
      case NIMF_KEY_KP_Up:
        {
          nimf_candidatable_select_previous_item (anthy->candidatable);
          gint index = nimf_candidatable_get_selected_index (anthy->candidatable);

          if (G_LIKELY (index >= 0))
            on_candidate_clicked (engine, target, NULL, index);
        }
        return TRUE;
      case NIMF_KEY_Down:
      case NIMF_KEY_KP_Down:
      case NIMF_KEY_space:
        {
          nimf_candidatable_select_next_item (anthy->candidatable);
          gint index = nimf_candidatable_get_selected_index (anthy->candidatable);

          if (G_LIKELY (index >= 0))
            on_candidate_clicked (engine, target, NULL, index);
        }
        return TRUE;
      case NIMF_KEY_Left:
      case NIMF_KEY_KP_Left:
        if (anthy->current_segment > 0)
        {
          anthy->current_segment--;
        }
        else
        {
          struct anthy_conv_stat conv_stat;

          anthy_get_stat (anthy->context, &conv_stat);
          anthy->current_segment = conv_stat.nr_segment - 1;
        }

        nimf_anthy_convert_preedit_text (engine, target);
        nimf_anthy_update_candidate     (engine, target);
        return TRUE;
      case NIMF_KEY_Right:
      case NIMF_KEY_KP_Right:
        {
          struct anthy_conv_stat conv_stat;

          anthy_get_stat (anthy->context, &conv_stat);

          if (anthy->current_segment < conv_stat.nr_segment - 1)
            anthy->current_segment++;
          else
            anthy->current_segment = 0;
        }

        nimf_anthy_convert_preedit_text (engine, target);
        nimf_anthy_update_candidate     (engine, target);
        return TRUE;
      case NIMF_KEY_Page_Up:
      case NIMF_KEY_KP_Page_Up:
        nimf_anthy_page_up (engine, target);
        return TRUE;
      case NIMF_KEY_Page_Down:
      case NIMF_KEY_KP_Page_Down:
        nimf_anthy_page_down (engine, target);
        return TRUE;
      case NIMF_KEY_Home:
        nimf_anthy_page_home (engine, target);
        return TRUE;
      case NIMF_KEY_End:
        nimf_anthy_page_end (engine, target);
        return TRUE;
      case NIMF_KEY_0:
      case NIMF_KEY_1:
      case NIMF_KEY_2:
      case NIMF_KEY_3:
      case NIMF_KEY_4:
      case NIMF_KEY_5:
      case NIMF_KEY_6:
      case NIMF_KEY_7:
      case NIMF_KEY_8:
      case NIMF_KEY_9:
      case NIMF_KEY_KP_0:
      case NIMF_KEY_KP_1:
      case NIMF_KEY_KP_2:
      case NIMF_KEY_KP_3:
      case NIMF_KEY_KP_4:
      case NIMF_KEY_KP_5:
      case NIMF_KEY_KP_6:
      case NIMF_KEY_KP_7:
      case NIMF_KEY_KP_8:
      case NIMF_KEY_KP_9:
        {
          if (g_strcmp0 (anthy->method,"romaji"))
            break;

          if (anthy->current_page < 1)
            break;

          struct anthy_segment_stat segment_stat;
          gint i, n;

          if (event->key.keyval >= NIMF_KEY_0 &&
              event->key.keyval <= NIMF_KEY_9)
            n = (event->key.keyval - NIMF_KEY_0 + 9) % 10;
          else if (event->key.keyval >= NIMF_KEY_KP_0 &&
                   event->key.keyval <= NIMF_KEY_KP_9)
            n = (event->key.keyval - NIMF_KEY_KP_0 + 9) % 10;
          else
            break;

          i = (anthy->current_page - 1) * 10 + n;

          anthy_get_segment_stat (anthy->context, anthy->current_segment, &segment_stat);

          if (i < MIN (anthy->current_page * 10, segment_stat.nr_candidate))
          {
            on_candidate_clicked (engine, target, NULL, n);
            nimf_anthy_update_candidate (engine, target);
          }
        }
        return TRUE;
      case NIMF_KEY_Escape:
        nimf_candidatable_hide (anthy->candidatable);
        nimf_anthy_update_preedit_text (engine, target);
        return TRUE;
      default:
        nimf_candidatable_hide (anthy->candidatable);
        nimf_anthy_update_preedit_text (engine, target);
        break;
    }
  }

  if (anthy->preedit->len > 0)
  {
    if (G_UNLIKELY (nimf_event_matches (event, (const NimfKey **) anthy->hiragana_keys)))
    {
      gchar *converted;

      converted = nimf_anthy_convert_to (anthy, NTH_HIRAGANA_CANDIDATE);

      g_string_assign (anthy->preedit, converted);
      nimf_anthy_update_preedit_state (engine, target, anthy->preedit->str,
                                       g_utf8_strlen (anthy->preedit->str, -1));
      g_free (converted);

      return TRUE;
    }
    else if (G_UNLIKELY (nimf_event_matches (event, (const NimfKey **) anthy->katakana_keys)))
    {
      gchar *converted;

      converted = nimf_anthy_convert_to (anthy, NTH_KATAKANA_CANDIDATE);

      g_string_assign (anthy->preedit, converted);
      nimf_anthy_update_preedit_state (engine, target, anthy->preedit->str,
                                       g_utf8_strlen (anthy->preedit->str, -1));
      g_free (converted);

      return TRUE;
    }
  }

  if ((event->key.state & NIMF_MODIFIER_MASK) == NIMF_CONTROL_MASK ||
      (event->key.state & NIMF_MODIFIER_MASK) == (NIMF_CONTROL_MASK | NIMF_MOD2_MASK))
    return FALSE;

  if (event->key.keyval == NIMF_KEY_space)
  {
    if (anthy->preedit->len == 0)
    {
      nimf_candidatable_hide (anthy->candidatable);
      return FALSE;
    }

    anthy->current_segment = 0;
    struct anthy_conv_stat conv_stat;

    if (anthy->n_input_mode == COMMON)
      nimf_anthy_replace_last_n (anthy);

    anthy_set_string (anthy->context, anthy->preedit->str);
    anthy_get_stat (anthy->context, &conv_stat);
    anthy->current_segment = conv_stat.nr_segment - 1;
    anthy->selections = g_realloc_n (anthy->selections, conv_stat.nr_segment,
                                     sizeof (gint));
    memset (anthy->selections, 0, conv_stat.nr_segment * sizeof (gint));

    nimf_anthy_convert_preedit_text (engine, target);

    if (!nimf_candidatable_is_visible (anthy->candidatable))
      nimf_candidatable_show (anthy->candidatable, target,
                              !nimf_service_ic_get_use_preedit (target));

    anthy->current_segment = 0;
    nimf_anthy_convert_preedit_text (engine, target);
    nimf_anthy_update_candidate (engine, target);

    return TRUE;
  }

  if (event->key.keyval == NIMF_KEY_Return)
  {
    if (anthy->preedit->len > 0)
    {
      if (anthy->n_input_mode == COMMON)
        nimf_anthy_replace_last_n (anthy);

      nimf_anthy_reset (engine, target);
      retval = TRUE;
    }

    retval = FALSE;
  }
  else if (event->key.keyval == NIMF_KEY_BackSpace)
  {
    if (anthy->preedit_offset + anthy->preedit_dx > 0)
    {
      if (anthy->preedit_dx > 0)
      {
        g_string_erase (anthy->preedit, anthy->preedit_offset + anthy->preedit_dx - 1, 1);
        anthy->preedit_dx--;
      }
      else if (anthy->preedit_offset > 0)
      {
        const gchar *prev;

        prev = g_utf8_prev_char (anthy->preedit->str + anthy->preedit_offset);
        g_string_erase (anthy->preedit, prev - anthy->preedit->str,
                        anthy->preedit->str + anthy->preedit_offset - prev);
        anthy->preedit_offset = prev - anthy->preedit->str;
      }

      retval = TRUE;
    }
    else
    {
      retval = FALSE;
    }
  }
  else if (event->key.keyval == NIMF_KEY_Delete ||
           event->key.keyval == NIMF_KEY_KP_Delete)
  {
    if (anthy->preedit_offset + anthy->preedit_dx < anthy->preedit->len)
    {
      const gchar *next;
      gint         len;

      next = g_utf8_next_char (anthy->preedit->str + anthy->preedit_offset + anthy->preedit_dx);
      len = next - (anthy->preedit->str + anthy->preedit_offset + anthy->preedit_dx);

      g_string_erase (anthy->preedit, anthy->preedit_offset + anthy->preedit_dx, len);

      retval = TRUE;
    }
    else
    {
      retval = FALSE;
    }
  }
  else if (event->key.keyval == NIMF_KEY_Left ||
           event->key.keyval == NIMF_KEY_KP_Left)
  {
    if (anthy->preedit_dx > 0)
    {
      anthy->preedit_offset += anthy->preedit_dx - 1;
      anthy->preedit_dx = 0;

      retval = TRUE;
    }
    else if (anthy->preedit_offset > 0)
    {
      const gchar *prev;

      prev = g_utf8_prev_char (anthy->preedit->str + anthy->preedit_offset);
      anthy->preedit_offset = prev - anthy->preedit->str;

      retval = TRUE;
    }
    else
    {
      retval = FALSE;
    }
  }
  else if (event->key.keyval == NIMF_KEY_Right ||
           event->key.keyval == NIMF_KEY_KP_Right)
  {
    const gchar *next;

    if (anthy->preedit_offset + anthy->preedit_dx < anthy->preedit->len)
    {
      next = g_utf8_next_char (anthy->preedit->str + anthy->preedit_offset + anthy->preedit_dx);
      anthy->preedit_offset = next - anthy->preedit->str;
      anthy->preedit_dx = 0;

      retval = TRUE;
    }
    else
    {
      retval = FALSE;
    }
  }
  else if (event->key.keyval > 127)
    retval = FALSE;
  else if (g_strcmp0 (anthy->method, "romaji") == 0)
    retval = nimf_anthy_filter_event_romaji (engine, target, event);
  else
    retval = nimf_anthy_filter_event_pc104 (engine, target, event);

  if (retval == FALSE)
    return FALSE;

  nimf_anthy_update_preedit_text (engine, target);

  return TRUE;
}

static void
on_changed_keys (GSettings *settings,
                 gchar     *key,
                 NimfAnthy *anthy)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gchar **keys = g_settings_get_strv (settings, key);

  if (g_strcmp0 (key, "hiragana-keys") == 0)
  {
    nimf_key_freev (anthy->hiragana_keys);
    anthy->hiragana_keys = nimf_key_newv ((const gchar **) keys);
  }
  else if (g_strcmp0 (key, "katakana-keys") == 0)
  {
    nimf_key_freev (anthy->katakana_keys);
    anthy->katakana_keys = nimf_key_newv ((const gchar **) keys);
  }

  g_strfreev (keys);
}

static void
on_changed_method (GSettings *settings,
                   gchar     *key,
                   NimfAnthy *anthy)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_free (anthy->method);
  anthy->method = g_settings_get_string (settings, key);
  anthy->method_changed = TRUE;
}

static gint
nimf_anthy_get_n_input_mode (NimfAnthy *anthy)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gchar *mode;
  gint   retval;

  mode = g_settings_get_string (anthy->settings, "get-n-input-mode-list");

  if (g_strcmp0 (mode, "common") == 0)
    retval = COMMON;
  else
    retval = EXPLICIT;

  g_free (mode);

  return retval;
}

static void
on_changed_n_input_mode (GSettings *settings,
                         gchar     *key,
                         NimfAnthy *anthy)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gchar *mode;

  mode = g_settings_get_string (settings, key);

  if (g_strcmp0 (mode, "common") == 0)
    anthy->n_input_mode = COMMON;
  else
    anthy->n_input_mode = EXPLICIT;

  g_free (mode);
}

static void
nimf_anthy_init (NimfAnthy *anthy)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  gchar **hiragana_keys;
  gchar **katakana_keys;

  anthy->id               = g_strdup ("nimf-anthy");
  anthy->preedit          = g_string_new ("");
  anthy->preedit_attrs    = g_malloc0_n (3, sizeof (NimfPreeditAttr *));
  anthy->preedit_attrs[0] = nimf_preedit_attr_new (NIMF_PREEDIT_ATTR_UNDERLINE, 0, 0);
  anthy->preedit_attrs[1] = nimf_preedit_attr_new (NIMF_PREEDIT_ATTR_HIGHLIGHT, 0, 0);
  anthy->preedit_attrs[2] = NULL;
  anthy->selections = g_malloc0_n (16, sizeof (gint));

  if (nimf_anthy_romaji)
  {
    g_hash_table_ref (nimf_anthy_romaji);
  }
  else
  {
    nimf_anthy_romaji = g_hash_table_new (g_str_hash, g_str_equal);

    gint i;
    for (i = 0; i < G_N_ELEMENTS (romaji_map); i++)
      g_hash_table_insert (nimf_anthy_romaji, romaji_map[i].key,
                                              romaji_map[i].value);
  }

  if (anthy_init () < 0)
    g_error (G_STRLOC ": %s: anthy is not initialized", G_STRFUNC);

  /* FIXME */
  /* anthy_set_personality () */
  anthy->context = anthy_create_context ();
  nimf_anthy_ref_count++;
  anthy_context_set_encoding (anthy->context, ANTHY_UTF8_ENCODING);

  anthy->settings = g_settings_new ("org.nimf.engines.nimf-anthy");
  anthy->method   = g_settings_get_string (anthy->settings, "get-method-infos");
  anthy->n_input_mode = nimf_anthy_get_n_input_mode (anthy);
  hiragana_keys = g_settings_get_strv   (anthy->settings, "hiragana-keys");
  katakana_keys = g_settings_get_strv   (anthy->settings, "katakana-keys");
  anthy->hiragana_keys = nimf_key_newv ((const gchar **) hiragana_keys);
  anthy->katakana_keys = nimf_key_newv ((const gchar **) katakana_keys);

  g_strfreev (hiragana_keys);
  g_strfreev (katakana_keys);

  g_signal_connect_data (anthy->settings, "changed::hiragana-keys",
          G_CALLBACK (on_changed_keys), anthy, NULL, G_CONNECT_AFTER);
  g_signal_connect_data (anthy->settings, "changed::katakana-keys",
          G_CALLBACK (on_changed_keys), anthy, NULL, G_CONNECT_AFTER);
  g_signal_connect_data (anthy->settings, "changed::get-method-infos",
          G_CALLBACK (on_changed_method), anthy, NULL, G_CONNECT_AFTER);
  g_signal_connect_data (anthy->settings, "changed::get-n-input-mode-list",
          G_CALLBACK (on_changed_n_input_mode), anthy, NULL, G_CONNECT_AFTER);
}

static void
nimf_anthy_finalize (GObject *object)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (object);

  nimf_preedit_attr_freev (anthy->preedit_attrs);
  g_free (anthy->id);
  g_free (anthy->selections);
  g_hash_table_unref (nimf_anthy_romaji);
  g_string_free (anthy->preedit, TRUE);
  nimf_key_freev (anthy->hiragana_keys);
  nimf_key_freev (anthy->katakana_keys);
  g_free (anthy->method);
  g_object_unref (anthy->settings);

  if (--nimf_anthy_ref_count == 0)
  {
    nimf_anthy_romaji = NULL;
    anthy_release_context (anthy->context);
    anthy_quit ();
  }

  G_OBJECT_CLASS (nimf_anthy_parent_class)->finalize (object);
}

const gchar *
nimf_anthy_get_id (NimfEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_val_if_fail (NIMF_IS_ENGINE (engine), NULL);

  return NIMF_ANTHY (engine)->id;
}

const gchar *
nimf_anthy_get_icon_name (NimfEngine *engine)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_return_val_if_fail (NIMF_IS_ENGINE (engine), NULL);

  return NIMF_ANTHY (engine)->id;
}

void
nimf_anthy_set_method (NimfEngine *engine, const gchar *method_id)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  g_settings_set_string (NIMF_ANTHY (engine)->settings,
                         "get-method-infos", method_id);
}

static void
nimf_anthy_constructed (GObject *object)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfAnthy *anthy = NIMF_ANTHY (object);

  anthy->candidatable = nimf_engine_get_candidatable (NIMF_ENGINE (anthy));
}

static void
nimf_anthy_class_init (NimfAnthyClass *class)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  GObjectClass *object_class = G_OBJECT_CLASS (class);
  NimfEngineClass *engine_class = NIMF_ENGINE_CLASS (class);

  engine_class->filter_event       = nimf_anthy_filter_event;
  engine_class->reset              = nimf_anthy_reset;
  engine_class->focus_in           = nimf_anthy_focus_in;
  engine_class->focus_out          = nimf_anthy_focus_out;

  engine_class->candidate_page_up   = nimf_anthy_page_up;
  engine_class->candidate_page_down = nimf_anthy_page_down;
  engine_class->candidate_clicked   = on_candidate_clicked;
  engine_class->candidate_scrolled  = on_candidate_scrolled;

  engine_class->get_id        = nimf_anthy_get_id;
  engine_class->get_icon_name = nimf_anthy_get_icon_name;
  engine_class->set_method    = nimf_anthy_set_method;

  object_class->constructed = nimf_anthy_constructed;
  object_class->finalize    = nimf_anthy_finalize;
}

static void
nimf_anthy_class_finalize (NimfAnthyClass *class)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);
}

typedef struct {
  const gchar *id;
  const gchar *name;
} Method;

static const Method methods[] = {
  {"romaji", N_("Romaji")},
  {"pc104",  N_("English Keyboard (pc104)")}
};

NimfMethodInfo **
nimf_anthy_get_method_infos ()
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfMethodInfo **infos;
  gint             n_methods = G_N_ELEMENTS (methods);
  gint             i;

  infos = g_malloc (sizeof (NimfMethodInfo *) * n_methods + 1);

  for (i = 0; i < n_methods; i++)
  {
    infos[i] = nimf_method_info_new ();
    infos[i]->method_id = g_strdup (methods[i].id);
    infos[i]->label     = g_strdup (gettext (methods[i].name));
    infos[i]->group     = NULL;
  }

  infos[n_methods] = NULL;

  return infos;
}

static const Method modes[] = {
  {"common",   N_("Common practice")},
  {"explicit", N_("Explicitly type nn")}
};

NimfMethodInfo **
nimf_anthy_get_n_input_mode_list ()
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  NimfMethodInfo **infos;
  gint             n_methods = G_N_ELEMENTS (modes);
  gint             i;

  infos = g_malloc (sizeof (NimfMethodInfo *) * n_methods + 1);

  for (i = 0; i < n_methods; i++)
  {
    infos[i] = nimf_method_info_new ();
    infos[i]->method_id = g_strdup (modes[i].id);
    infos[i]->label     = g_strdup (gettext (modes[i].name));
    infos[i]->group     = NULL;
  }

  infos[n_methods] = NULL;

  return infos;
}

void
module_register_type (GTypeModule *type_module)
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  nimf_anthy_register_type (type_module);
}

GType
module_get_type ()
{
  g_debug (G_STRLOC ": %s", G_STRFUNC);

  return nimf_anthy_get_type ();
}
