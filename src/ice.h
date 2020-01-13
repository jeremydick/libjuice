/**
 * Copyright (c) 2020 Paul-Louis Ageneau
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef JUICE_ICE_H
#define JUICE_ICE_H

#include "addr.h"
#include "juice.h"

#include <stdbool.h>
#include <stdint.h>

#define ICE_MAX_CANDIDATES_COUNT 16

typedef enum ice_candidate_type {
	ICE_CANDIDATE_TYPE_HOST,
	ICE_CANDIDATE_TYPE_SERVER_REFLEXIVE,
	ICE_CANDIDATE_TYPE_PEER_REFLEXIVE,
	ICE_CANDIDATE_TYPE_RELAYED,
} ice_candidate_type_t;

// RFC 8445: The RECOMMENDED values for type preferences are 126 for host
// candidates, 110 for peer-reflexive candidates, 100 for server-reflexive
// candidates, and 0 for relayed candidates.
#define ICE_CANDIDATE_PREF_HOST 126
#define ICE_CANDIDATE_PREF_PEER_REFLEXIVE 110
#define ICE_CANDIDATE_PREF_SERVER_REFLEXIVE 100
#define ICE_CANDIDATE_PREF_RELAYED 0

typedef struct ice_candidate {
	ice_candidate_type_t type;
	uint32_t priority;
	int component;
	char foundation[32 + 1]; // 1 to 32 characters
	char transport[32 + 1];
	char hostname[256 + 1];
	char service[32 + 1];
	struct sockaddr_record resolved;
} ice_candidate_t;

typedef struct ice_description {
	char ice_ufrag[256 + 1]; // 4 to 256 characters
	char ice_pwd[256 + 1];   // 22 to 256 characters
	ice_candidate_t candidates[ICE_MAX_CANDIDATES_COUNT];
	size_t candidates_count;
} ice_description_t;

typedef struct ice_candidate_pair {
	ice_candidate_t *local;
	ice_candidate_t *remote;
	uint64_t priority;
} ice_candidate_pair_t;

typedef enum ice_resolve_mode {
	ICE_RESOLVE_MODE_SIMPLE,
	ICE_RESOLVE_MODE_LOOKUP,
} ice_resolve_mode_t;

int ice_parse_sdp(const char *sdp, ice_description_t *description);
int ice_parse_candidate_sdp(const char *line, ice_candidate_t *candidate);
int ice_create_local_description(ice_description_t *description);
int ice_create_local_candidate(ice_candidate_type_t type, int component,
                               const struct sockaddr_record *record,
                               ice_candidate_t *candidate);
int ice_resolve_candidate(ice_candidate_t *candidate, ice_resolve_mode_t mode);
int ice_add_candidate(const ice_candidate_t *candidate,
                      ice_description_t *description);
void ice_sort_candidates(ice_description_t *description);
const ice_candidate_t *
ice_find_candidate_from_addr(const ice_description_t *description,
                             const struct sockaddr_record *record);
int ice_generate_sdp(const ice_description_t *description, char *buffer,
                     size_t size);
int ice_generate_candidate_sdp(const ice_candidate_t *candidate, char *buffer,
                               size_t size);
int ice_create_candidate_pair(ice_candidate_t *local, ice_candidate_t *remote,
                              bool is_controlling, ice_candidate_pair_t *pair);

#endif
