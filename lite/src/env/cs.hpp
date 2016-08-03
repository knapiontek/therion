
core::String cs_F = cs("F: ");
core::String cs_E = cs("E: ");
core::String cs_W = cs("W: ");
core::String cs_I = cs("I: ");
core::String cs_D = cs("D: ");

core::String cs_abort = cs("captured signal: Abort");
core::String cs_fpe = cs("captured signal: Floating-Point Exception");
core::String cs_ill = cs("captured signal: Illegal Instruction");
core::String cs_interrupt = cs("captured signal: Interrupt");
core::String cs_segv = cs("captured signal: Segmentation Violation");
core::String cs_terminate = cs("captured signal: Terminate");

core::String cs_allocation_exception = cs("allocation exception");
core::String cs_assert = cs("\nassert:\n\t$1 [$2:$3]\nbacktrace:");

core::String cs_line = cs("\n");
core::String cs_line_tab = cs("\n\t");
core::String cs_frame = cs("\n\t$1 [$2:$3]");

core::String cs_exception =
    cs_eng("\nexception:")
    cs_pl("\nwyjątek:");

core::String cs_context =
    cs_eng("\ncontext:")
    cs_pl("\ncontekst:");

core::String cs_backtrace =
    cs_eng("\nbacktrace:")
    cs_pl("\nstos wykonania:");
