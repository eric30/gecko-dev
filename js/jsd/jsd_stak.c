/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 */

/*
 * JavaScript Debugging support - Call stack support
 */

#include "jsd.h"

#ifdef DEBUG
void JSD_ASSERT_VALID_THREAD_STATE(JSDThreadState* jsdthreadstate)
{
    JS_ASSERT(jsdthreadstate);
    JS_ASSERT(jsdthreadstate->stackDepth > 0);
}

void JSD_ASSERT_VALID_STACK_FRAME(JSDStackFrameInfo* jsdframe)
{
    JS_ASSERT(jsdframe);
    JS_ASSERT(jsdframe->jsdthreadstate);
}
#endif

static JSDStackFrameInfo* 
_addNewFrame(JSDContext*        jsdc,
             JSDThreadState*    jsdthreadstate,
             JSScript*          script,
             jsuword            pc,
             JSStackFrame*      fp)
{
    JSDStackFrameInfo* jsdframe;
    JSDScript*         jsdscript;

    JSD_LOCK_SCRIPTS(jsdc);
    jsdscript = jsd_FindJSDScript(jsdc, script);
    JSD_UNLOCK_SCRIPTS(jsdc);
    if( ! jsdscript )
        return NULL;

    jsdframe = (JSDStackFrameInfo*) calloc(1, sizeof(JSDStackFrameInfo));
    if( ! jsdframe )
        return NULL;

    jsdframe->jsdthreadstate = jsdthreadstate;
    jsdframe->jsdscript      = jsdscript     ;
    jsdframe->pc             = pc            ;
    jsdframe->fp             = fp            ;

    JS_APPEND_LINK(&jsdframe->links, &jsdthreadstate->stack);
    jsdthreadstate->stackDepth++;

    return jsdframe;
}

static void
_destroyFrame(JSDStackFrameInfo* jsdframe)
{
    /* kill any alloc'd objects in frame here... */

    if( jsdframe )
        free(jsdframe);
}


JSDThreadState*
jsd_NewThreadState(JSDContext* jsdc, JSContext *cx )
{
    JSDThreadState* jsdthreadstate;
    JSStackFrame *  iter = NULL;
    JSStackFrame *  fp;

    jsdthreadstate = (JSDThreadState*)calloc(1, sizeof(JSDThreadState));
    if( ! jsdthreadstate )
        return NULL;

    jsdthreadstate->context = cx;
    jsdthreadstate->thread = JSD_CURRENT_THREAD();
    JS_INIT_CLIST(&jsdthreadstate->stack);
    jsdthreadstate->stackDepth = 0;

    while( NULL != (fp = JS_FrameIterator(cx, &iter)) )
    {
        JSScript* script = JS_GetFrameScript(cx, fp);
        jsuword  pc = (jsuword) JS_GetFramePC(cx, fp);

        if( ! script || ! pc || JS_IsNativeFrame(cx, fp) )
            continue;

        _addNewFrame( jsdc, jsdthreadstate, script, pc, fp );
    }
    
    /* if there is no stack, then this threadstate can not be constructed */
    if( 0 == jsdthreadstate->stackDepth )
    {
        free(jsdthreadstate);
        return NULL;
    }

    JSD_LOCK_THREADSTATES(jsdc);
    JS_APPEND_LINK(&jsdthreadstate->links, &jsdc->threadsStates);
    JSD_UNLOCK_THREADSTATES(jsdc);

    return jsdthreadstate;
}

void
jsd_DestroyThreadState(JSDContext* jsdc, JSDThreadState* jsdthreadstate)
{
    JSDStackFrameInfo* jsdframe;
    JSCList* list;

    JSD_ASSERT_VALID_THREAD_STATE(jsdthreadstate);
    JS_ASSERT(JSD_CURRENT_THREAD() == jsdthreadstate->thread);

    JSD_LOCK_THREADSTATES(jsdc);
    JS_REMOVE_LINK(&jsdthreadstate->links);
    JSD_UNLOCK_THREADSTATES(jsdc);

    list = &jsdthreadstate->stack;
    while( (JSDStackFrameInfo*)list != (jsdframe = (JSDStackFrameInfo*)list->next) )
    {
        JS_REMOVE_LINK(&jsdframe->links);
        _destroyFrame(jsdframe);
    }
    free(jsdthreadstate);
}

uintN
jsd_GetCountOfStackFrames(JSDContext* jsdc, JSDThreadState* jsdthreadstate)
{
    uintN count = 0;

    JSD_LOCK_THREADSTATES(jsdc);

    if( jsd_IsValidThreadState(jsdc, jsdthreadstate) )
        count = jsdthreadstate->stackDepth;

    JSD_UNLOCK_THREADSTATES(jsdc);

    return count;
}

JSDStackFrameInfo*
jsd_GetStackFrame(JSDContext* jsdc, JSDThreadState* jsdthreadstate)
{
    JSDStackFrameInfo* jsdframe = NULL;

    JSD_LOCK_THREADSTATES(jsdc);

    if( jsd_IsValidThreadState(jsdc, jsdthreadstate) )
        jsdframe = (JSDStackFrameInfo*) JS_LIST_HEAD(&jsdthreadstate->stack);
    JSD_UNLOCK_THREADSTATES(jsdc);

    return jsdframe;
}

JSDStackFrameInfo*
jsd_GetCallingStackFrame(JSDContext* jsdc, 
                         JSDThreadState* jsdthreadstate,
                         JSDStackFrameInfo* jsdframe)
{
    JSDStackFrameInfo* nextjsdframe = NULL;

    JSD_LOCK_THREADSTATES(jsdc);

    if( jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe) )
        if( JS_LIST_HEAD(&jsdframe->links) != &jsdframe->jsdthreadstate->stack )
            nextjsdframe = (JSDStackFrameInfo*) JS_LIST_HEAD(&jsdframe->links);

    JSD_UNLOCK_THREADSTATES(jsdc);

    return nextjsdframe;
}

JSDScript*
jsd_GetScriptForStackFrame(JSDContext* jsdc, 
                           JSDThreadState* jsdthreadstate,
                           JSDStackFrameInfo* jsdframe)
{
    JSDScript* jsdscript = NULL;

    JSD_LOCK_THREADSTATES(jsdc);

    if( jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe) )
        jsdscript = jsdframe->jsdscript;

    JSD_UNLOCK_THREADSTATES(jsdc);

    return jsdscript;
}

jsuword
jsd_GetPCForStackFrame(JSDContext* jsdc, 
                       JSDThreadState* jsdthreadstate,
                       JSDStackFrameInfo* jsdframe)
{
    jsuword pc = 0;

    JSD_LOCK_THREADSTATES(jsdc);

    if( jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe) )
        pc = jsdframe->pc;

    JSD_UNLOCK_THREADSTATES(jsdc);

    return pc;
}

JSDValue*
jsd_GetCallObjectForStackFrame(JSDContext* jsdc, 
                               JSDThreadState* jsdthreadstate,
                               JSDStackFrameInfo* jsdframe)
{
    JSObject* obj;
    JSDValue* jsdval = NULL;

    JSD_LOCK_THREADSTATES(jsdc);

    if( jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe) )
    {
        obj = JS_GetFrameCallObject(jsdthreadstate->context, jsdframe->fp); 
        if(obj)                                                             
            jsdval = JSD_NewValue(jsdc, OBJECT_TO_JSVAL(obj));              
    }

    JSD_UNLOCK_THREADSTATES(jsdc);

    return jsdval;
}

JSDValue*
jsd_GetScopeChainForStackFrame(JSDContext* jsdc, 
                               JSDThreadState* jsdthreadstate,
                               JSDStackFrameInfo* jsdframe)
{
    JSObject* obj;
    JSDValue* jsdval = NULL;

    JSD_LOCK_THREADSTATES(jsdc);

    if( jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe) )
    {
        obj = JS_GetFrameScopeChain(jsdthreadstate->context, jsdframe->fp); 
        if(obj)                                                             
            jsdval = JSD_NewValue(jsdc, OBJECT_TO_JSVAL(obj));              
    }

    JSD_UNLOCK_THREADSTATES(jsdc);

    return jsdval;
}

JSDValue*
jsd_GetThisForStackFrame(JSDContext* jsdc, 
                         JSDThreadState* jsdthreadstate,
                         JSDStackFrameInfo* jsdframe)
{
    JSObject* obj;
    JSDValue* jsdval = NULL;

    JSD_LOCK_THREADSTATES(jsdc);

    if( jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe) )
    {
        obj = JS_GetFrameThis(jsdthreadstate->context, jsdframe->fp);
        if(obj)
            jsdval = JSD_NewValue(jsdc, OBJECT_TO_JSVAL(obj));
    }

    JSD_UNLOCK_THREADSTATES(jsdc);

    return jsdval;
}

JSBool
jsd_EvaluateUCScriptInStackFrame(JSDContext* jsdc, 
                                 JSDThreadState* jsdthreadstate,
                                 JSDStackFrameInfo* jsdframe,
                                 const jschar *bytes, uintN length,
                                 const char *filename, uintN lineno, jsval *rval)
{
    JSBool retval;
    JSBool valid;
    JSExceptionState* exceptionState;
    JSContext* cx;

    JS_ASSERT(JSD_CURRENT_THREAD() == jsdthreadstate->thread);

    JSD_LOCK_THREADSTATES(jsdc);
    valid = jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe);
    JSD_UNLOCK_THREADSTATES(jsdc);

    if( ! valid )
        return JS_FALSE;

    cx = jsdthreadstate->context;
    JS_ASSERT(cx);

    exceptionState = JS_SaveExceptionState(cx);
    jsd_StartingEvalUsingFilename(jsdc, filename);
    retval = JS_EvaluateUCInStackFrame(cx, jsdframe->fp, bytes, length, 
                                       filename, lineno, rval);
    jsd_FinishedEvalUsingFilename(jsdc, filename);
    JS_RestoreExceptionState(cx, exceptionState);

    return retval;
}

JSBool
jsd_EvaluateScriptInStackFrame(JSDContext* jsdc, 
                               JSDThreadState* jsdthreadstate,
                               JSDStackFrameInfo* jsdframe,
                               const char *bytes, uintN length,
                               const char *filename, uintN lineno, jsval *rval)
{
    JSBool retval;
    JSBool valid;
    JSExceptionState* exceptionState;
    JSContext *cx;

    JS_ASSERT(JSD_CURRENT_THREAD() == jsdthreadstate->thread);

    JSD_LOCK_THREADSTATES(jsdc);
    valid = jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe);
    JSD_UNLOCK_THREADSTATES(jsdc);

    if (!valid)
        return JS_FALSE;

    cx = jsdthreadstate->context;
    JS_ASSERT(cx);

    exceptionState = JS_SaveExceptionState(cx);
    jsd_StartingEvalUsingFilename(jsdc, filename);
    retval = JS_EvaluateInStackFrame(cx, jsdframe->fp, bytes, length,
                                     filename, lineno, rval);
    jsd_FinishedEvalUsingFilename(jsdc, filename);
    JS_RestoreExceptionState(cx, exceptionState);

    return retval;
}

JSString*
jsd_ValToStringInStackFrame(JSDContext* jsdc, 
                            JSDThreadState* jsdthreadstate,
                            JSDStackFrameInfo* jsdframe,
                            jsval val)
{
    JSBool valid;
    JSString* retval;
    JSExceptionState* exceptionState;
    JSContext* cx;

    JSD_LOCK_THREADSTATES(jsdc);
    valid = jsd_IsValidFrameInThreadState(jsdc, jsdthreadstate, jsdframe);
    JSD_UNLOCK_THREADSTATES(jsdc);

    if( ! valid )
        return NULL;

    cx = jsdthreadstate->context;
    JS_ASSERT(cx);

    exceptionState = JS_SaveExceptionState(cx);
    retval = JS_ValueToString(cx, val);
    JS_RestoreExceptionState(cx, exceptionState);

    return retval;
}

JSBool
jsd_IsValidThreadState(JSDContext*        jsdc, 
                       JSDThreadState*    jsdthreadstate)
{
    JSDThreadState *cur;

    JS_ASSERT( JSD_THREADSTATES_LOCKED(jsdc) );

    for( cur = (JSDThreadState*)jsdc->threadsStates.next;
         cur != (JSDThreadState*)&jsdc->threadsStates;
         cur = (JSDThreadState*)cur->links.next ) 
    {
        if( cur == jsdthreadstate )
            return JS_TRUE;
    }
    return JS_FALSE;
}    

JSBool
jsd_IsValidFrameInThreadState(JSDContext*        jsdc, 
                              JSDThreadState*    jsdthreadstate,
                              JSDStackFrameInfo* jsdframe)
{
    JS_ASSERT(JSD_THREADSTATES_LOCKED(jsdc));

    if( ! jsd_IsValidThreadState(jsdc, jsdthreadstate) )
        return JS_FALSE;
    if( jsdframe->jsdthreadstate != jsdthreadstate )
        return JS_FALSE;

    JSD_ASSERT_VALID_THREAD_STATE(jsdthreadstate);
    JSD_ASSERT_VALID_STACK_FRAME(jsdframe);
    
    return JS_TRUE;
}

static JSContext*
_getContextForThreadState(JSDContext* jsdc, JSDThreadState* jsdthreadstate)
{
    JSBool valid;
    JSD_LOCK_THREADSTATES(jsdc);
    valid = jsd_IsValidThreadState(jsdc, jsdthreadstate);
    JSD_UNLOCK_THREADSTATES(jsdc);
    if( valid )
        return jsdthreadstate->context;
    return NULL;
}        

JSDValue*
jsd_GetException(JSDContext* jsdc, JSDThreadState* jsdthreadstate)
{
    JSContext* cx;
    jsval val;

    if(!(cx = _getContextForThreadState(jsdc, jsdthreadstate)))
        return NULL;

    if(JS_GetPendingException(cx, &val))
        return jsd_NewValue(jsdc, val);
    return NULL;
}        

JSBool
jsd_SetException(JSDContext* jsdc, JSDThreadState* jsdthreadstate, 
                 JSDValue* jsdval)
{
    JSContext* cx;

    if(!(cx = _getContextForThreadState(jsdc, jsdthreadstate)))
        return JS_FALSE;

    if(jsdval)
        JS_SetPendingException(cx, JSD_GetValueWrappedJSVal(jsdc, jsdval));
    else
        JS_ClearPendingException(cx);
    return JS_TRUE;
}

