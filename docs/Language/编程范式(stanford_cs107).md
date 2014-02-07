第二课
=======



<!--language: !c-->

    #include <malloc.h>
    #include <string.h>
    #include <stdio.h>

    void swap(void *vp1, void *vp2, int size){
        char buffer[size];
        memcpy(buffer, vp1, size);
        memcpy(vp1, vp2, size);
        memcpy(vp2, buffer, size);
    }

    int main(){
        double a = 23.0, b = 34.0;
        swap(&a, &b, sizeof(double));
        printf("%lf %lf", a, b);
    }

<!--language: !c-->

    #include <malloc.h>
    #include <string.h>
    #include <stdio.h>

    void swap(void *vp1, void *vp2, int size){
        char buffer[size];
        memcpy(buffer, vp1, size);
        memcpy(vp1, vp2, size);
        memcpy(vp2, buffer, size);
    }

    int main(){
        char *husband = strdup("Fred"); char *wife = strdup("Wilma");
        swap(&husband, &wife, sizeof(char *));
        printf("%s %s", husband, wife);
    }

http://www.cppblog.com/deercoder/archive/2012/06/24/180038.html