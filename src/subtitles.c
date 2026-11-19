#include "subtitles.h"

// Initialize the subtitle system
SUBTITLES *SUBTITLES_CREATE() 
{
    SUBTITLES *v = malloc(sizeof(SUBTITLES));
    if(!v)
    {
      return NULL;
    }
    memset(v, 0, sizeof(SUBTITLES));
    v->active = false;
    v->timeremaining = 0.0f;
    v->text[0] = '\0';
    return v;
}

void SUBTITLES_DESTROY(SUBTITLES *v) 
{
  if(!v)
  {
    return;
  }
  free(v);
}

void SUBTITLES_PUSH(SUBTITLES *v, char *text) 
{
    strncpy(v->text, text, MAX_SUBTITLE_LENGTH - 1);
    v->text[MAX_SUBTITLE_LENGTH - 1] = '\0';
    v->timeremaining = SUBTITLE_DURATION_SEC;
    v->active = true;
}

// Update the timer each frame
void SUBTITLES_UPDATE(SUBTITLES *v, float dt) 
{
    if (v->active) 
    {
        v->timeremaining -= dt;
        if (v->timeremaining <= 0.0f) 
        {
            v->active = false;
        }
    }
}

// Render the subtitle centered at the bottom of the screen with a dark backdrop
void SUBTITLES_DRAW(SUBTITLES *v, int screenwidth, int screenheight) {
    if (!v->active) return;

    int fontsize = 20;
    int textwidth = MeasureText(v->text, fontsize);
    int posx = (screenwidth - textwidth) / 2;
    int posy = screenheight - 70;
    int padding = 12;

    // Semi-transparent backdrop for legibility
    DrawRectangle(
        posx - padding, 
        posy - padding, 
        textwidth + (padding * 2), 
        fontsize + (padding * 2), 
        Fade(BLACK, 0.65f)
    );

    // Subtitle text
    DrawText(v->text, posx, posy, fontsize, WHITE);
}


