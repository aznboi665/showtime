/*
 *  Video decoder
 *  Copyright (C) 2007 - 2010 Andreas Öman
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLW_VIDEO_COMMON_H
#define GLW_VIDEO_COMMON_H

#include "glw.h"
#include "media.h"
#if ENABLE_DVD
#include "video/video_dvdspu.h"
#endif
#include "video/video_playback.h"

int glw_video_pointer_event(dvdspu_decoder_t *dd, int width, int height,
			    glw_pointer_event_t *gpe, media_pipe_t *mp);

void glw_video_update_focusable(video_decoder_t *vd, glw_t *w);

int glw_video_widget_event(event_t *e, media_pipe_t *mp, int in_menu);

int glw_video_compute_output_duration(video_decoder_t *vd, int frame_duration);

void glw_video_compute_avdiff(video_decoder_t *vd, media_pipe_t *mp, 
			      int64_t pts, int epoch);



/**
 *
 */
static inline void
glw_video_enqueue_for_display(video_decoder_t *vd, video_decoder_frame_t *vdf,
			      struct video_decoder_frame_queue *fromqueue)
{
  TAILQ_REMOVE(fromqueue, vdf, vdf_link);
  TAILQ_INSERT_TAIL(&vd->vd_displaying_queue, vdf, vdf_link);
}

#endif /* GLW_VIDEO_COMMON_H */
