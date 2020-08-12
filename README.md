# ts_tools
adjust ts stream information by Qt
you can use the tools adjust (ts/mts) stream

including PSI(PAT,PMT,NIT,CAT)、SI(EIT,TDT,TOT,SDT/BAT)、Stream Basic information(Service id,Audio stream id,Vidio stream id) and simple EPG information(by different classification ways)、Section_data information(188 bytes)

Use database transaction processing to speed up data loading and turn on multithreading (Sqlite、QthreadPool）
