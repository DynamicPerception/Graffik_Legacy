/*
    Dynamic Perception MoCoBus Development Library
    Copyright (c) 2010-2012 Dynamic Perception LLC

    This file is part of The Dynamic Perception MoCoBus Development Library.

    The Dynamic Perception MoCoBus Development Library is free software:
    you can redistribute it and/or modify it under the terms of the GNU
    Lesser General Public License as published by the Free Software Foundation,
    either version 3 of the License, or (at your option) any later version.

    The Dynamic Perception MoCoBus Development Library is distributed in the
    hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
    implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with The Dynamic Perception MoCoBus Development Library.  If not,
    see <http://www.gnu.org/licenses/>.

  */

#ifndef OPENMOCO_H
#define OPENMOCO_H



/** Base OpenMoCo Class

  Defines core values for use in MoCoBus libraries

  @author
  C. A. Church

  (c) 2011-2012 Dynamic Perception LLC
  Licensed under the terms of the GNU LGPL version 3.
  */


#define OMC_FAILURE  0
#define OMC_SUCCESS  1
#define OMC_NOCMD    2
#define OMC_QUEUED   3
#define OMC_TIMEOUT  4

#define OM_ERR_ARG     2
#define OM_ERR_MEM     3
#define OM_ERR_QUEUE   4
#define OM_ERR_SERAVAIL 5
#define OM_ERR_NOCMD   6



#endif // OPENMOCO_H
