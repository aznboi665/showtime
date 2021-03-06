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

#ifndef GLW_VIDEO_OVERLAY_H
#define GLW_VIDEO_OVERLAY_H

void glw_video_overlay_deinit(glw_root_t *gr, glw_video_overlay_t *gvo);

void glw_video_overlay_render(glw_video_overlay_t *gvo, glw_root_t *gr,
			      glw_rctx_t *rc);

int glw_video_overlay_pointer_event(video_decoder_t *vd, int width, int height,
				    glw_pointer_event_t *gpe, media_pipe_t *mp);

void glw_video_overlay_layout(glw_video_t *gv, int64_t pts,
			      video_decoder_t *vd);

#endif // GLW_VIDEO_OVERLAY_H
