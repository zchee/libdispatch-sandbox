/*
 * Copyright (c) 2010-2011 Apple Inc. All rights reserved.
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#include <dispatch/dispatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static char *ctxts[] = {"ctxt for app key 0", "ctxt for key 1", "ctxt for key 2", "ctxt for key 3 bis",
												"ctxt for key 4"};
volatile long ctxts_destroyed;
static dispatch_group_t g;

static void destructor(void *ctxt) {
	fprintf(stderr, "Set!!!!!! %s\n", ( char * )ctxt);
	/* fprintf(stderr, "destructor of %s\n", ( char * )ctxt); */
	( void )__sync_add_and_fetch(&ctxts_destroyed, 1);
	dispatch_group_leave(g);
}

static void test_context_for_key(void) {
	// Create dispatch_group and set to g
	g = dispatch_group_create();

	// Create dispatch queue defined q, identifier DISPATCH_QUEUE_SERIAL
	// dispatch object
	dispatch_queue_t q = dispatch_queue_create("q", NULL);
	// Create dispatch queue defined tq, identifier DISPATCH_QUEUE_CONCURRENT
	// dispatch object
	dispatch_queue_t tq = dispatch_queue_create("tq", DISPATCH_QUEUE_CONCURRENT);
	// Get global dispatch queue defined ttq, identifier DISPATCH_QUEUE_SERIAL
	// dispatch queue
	dispatch_queue_t ttq = dispatch_get_global_queue(0, 0);

	// Update the number of blocks running within a group(g)
	// dispatch_group_leave is internal destrutor function
	dispatch_group_enter(g);

	// Set specific context to tq queue (DISPACH_QUEUE_CONCURENT)
	// key: "ctxt for key 4"
	// context: "ctxt for key 4"
	dispatch_queue_set_specific(tq, &ctxts[ 4 ], ctxts[ 4 ], destructor);

	// Set tq queue jobs to ttq(global queue)
	dispatch_set_target_queue(tq, ttq);

	// Update the number of blocks running within a group(g)
	// dispatch_group_leave is internal destrutor function
	dispatch_group_enter(g);

	// Set specific context to q queue (DISPATCH_QUEUE_SERIAL)
	// key: "ctxt for app key 0"
	// context: "ctxt for app key 0"
	dispatch_set_context(q, ctxts[ 0 ]);

	// Set q queue jobs to tq (DISPATCH_QUEUE_CONCURRENT)
	dispatch_set_target_queue(q, tq);

	// Sets the finalizer function for a q dispatch object
	/* dispatch_set_finalizer_f(q, destructor); */

	// Exection dispatch_group_enter and dispatch_queue_set_specific asynchronous
	/* dispatch_async(q, ^{ */
	/* 	dispatch_group_enter(g); */
	/* 	dispatch_queue_set_specific(q, &ctxts[ 1 ], ctxts[ 1 ], destructor); */
	/* }); */

	// Retain the q (DISPATCH_QUEUE_SERIAL) queue
	dispatch_retain(q);

	// Exection set and get specific context
	dispatch_async(dispatch_get_global_queue(0, 0), ^{
		dispatch_group_enter(g);
		/* dispatch_queue_set_specific(q, &ctxts[ 2 ], ctxts[ 2 ], destructor); */
		dispatch_async(dispatch_get_global_queue(0, 0), ^{
			void *ctxt;
			ctxt = dispatch_queue_get_specific(q, &ctxts[ 4 ]);
			fprintf(stderr, "get context for key 4 \n    key: %s\n", ctxts[ 4 ]);
			dispatch_release(q);
		});
	});

	/* dispatch_async(q, ^{ */
	/* 	void *ctxt; */
	/* 	ctxt = dispatch_get_specific(&ctxts[ 1 ]); */
	/* 	fprintf(stderr, "get current context for key 1 \n    key: %s\n", ctxts[ 1 ]); */
	/* 	ctxt = dispatch_get_specific(&ctxts[ 4 ]); */
	/* 	fprintf(stderr, "get current context for key 4 \n    key: %s\n", ctxts[ 4 ]); */
	/* 	ctxt = dispatch_queue_get_specific(q, &ctxts[ 1 ]); */
	/* 	fprintf(stderr, "get context for key 1 \n    key: %s\n", ctxts[ 1 ]); */
	/* }); */

	/* dispatch_async(q, ^{ */
	/* 	dispatch_group_enter(g); */
	/* 	void *ctxt; */
	/* 	dispatch_queue_set_specific(q, &ctxts[ 1 ], ctxts[ 3 ], destructor); */
	/* 	ctxt = dispatch_queue_get_specific(q, &ctxts[ 1 ]); */
	/* 	printf("get context for key 1 %s %s", ctxt, ctxts[ 3 ]); */
	/* }); */

	/* dispatch_async(q, ^{ */
	/* 	void *ctxt; */
	/* 	dispatch_queue_set_specific(q, &ctxts[ 1 ], NULL, destructor); */
	/* 	ctxt = dispatch_queue_get_specific(q, &ctxts[ 1 ]); */
	/* 	fprintf(stderr, "get context for key 1 \n    key: %s\n", ctxt); */
	/* }); */

	/* void *ctxt = dispatch_get_context(q); */
	/* fprintf(stderr, "get context for app \n    key: %s\n", ctxts[ 0 ]); */

	// Release the tq(DISPATCH_QUEUE_CONCURRENT) queue
	dispatch_release(tq);
	// Release the q(global queue)
	dispatch_release(q);

	dispatch_group_wait(g, DISPATCH_TIME_FOREVER);
	fprintf(stderr, "contexts destroyed %ld %d", ctxts_destroyed, 5);

	// Release the g(dispatch_group)
	dispatch_release(g);
}

int main(void) {
	dispatch_async(dispatch_get_main_queue(), ^{
		test_context_for_key();
	});
	dispatch_main();
}
