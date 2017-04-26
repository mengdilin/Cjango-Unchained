library(tidyverse)
library(stringr)
f <- 'cjango-c1-1.log'
out <- NULL
setwd("./src/bench/cygtest/all")
for (f in dir(".", pattern = "1.log")) {
    app_name <- gsub("-.*","", f)
    n_concurrent <- gsub(".*-c([0-9]*)-.*", "\\1", f, perl = TRUE)
    take <- gsub(".*-(.*)\\.log", "\\1", f, perl = TRUE)
    data.frame()
    
    app_name <- ifelse(app_name=="cjango", "Cjango (C++)",
                       ifelse(app_name=='crow', "Crow (C++, Github Star #1)",
                              ifelse(app_name=='django',"Django (Python)", "Silicon (C++, Star #2)")))
    
    a_row <- bind_cols(data.frame(  app = rep(app_name,10000),
                                   ncon = rep(n_concurrent, 10000) %>% as.integer,
                                   take = rep(take, 10000)),
                        read.delim(f, stringsAsFactors = FALSE))
    if (is.null(out))
        out <- a_row
    else
        out <- bind_rows(out, a_row)
    message(f)
}

out <- out %>% mutate( less15 = ifelse(ncon < 300, "less", "not") %>% as.factor())

res <- out %>% group_by(app, ncon) %>% filter(take==1 & (! ncon == 11) ) %>% summarise( ttime = mean(ttime), dtime = mean(dtime) )

library(ggforce)
library(ggbash)
ggplot(as.data.frame(res), aes(x=ncon, y=dtime, color=app, group=app)) + geom_line(size=1.5) + geom_point(size=9)+ geom_point(size=7, color="white") +
    geom_text(aes(label=substr(app,1,2)), fontface="bold") + facet_zoom(xlim=c(1,10.1), ylim=c(0,14)) + theme(legend.position = c(.2,.7)) + theme2(l.txt(sz=15, f="bold"), a.txt(size=18, f="bold"), a.ttl(f="bold", sz=20)) + labs(x="Number of Concurrent Connections", y="Total Time of Apache Bench [ms/req]")


ggplot(res, aes(x=as.character(ncon) %>% str_pad(pad="0", width=3), y=ttime, fill=reorder(app, -ttime) )) + geom_bar(stat="identity", position="dodge") 

ggplot(scalability, aes(x=log10(dtime), fill=app)) + geom_density(alpha=.4) + labs(x="Processing Time [log10(ms)]")+ theme(legend.position = c(.7,.7)) + theme2(l.txt(sz=15, f="bold"), a.txt(size=18, f="bold"), a.ttl(f="bold", sz=20))

ggplot(scalability, aes(x=log10(wait), fill=app)) + geom_density(alpha=.4) + labs(x="Waiting Time [log10(ms)]")+ theme(legend.position = c(.7,.7)) + theme2(l.txt(sz=15, f="bold"), a.txt(size=18, f="bold"), a.ttl(f="bold", sz=20))

scalability <- out %>% filter(take==1 & ncon==150)
ggplot(scalability, aes(x=log(dtime))) +
    geom_histogram(data=scalability%>% filter(app=="Cjango (C++)"), fill="red", alpha=.4) +
    geom_histogram(data=scalability%>% filter(app=="Crow (C++, Github Star #1)"), fill="blue", alpha=.4) +
    geom_histogram(data=scalability%>% filter(app=="Silicon (C++, Star #2)"), fill="green", alpha=.4)


