/**
 * vyvtoril Alexandr Chalupnik (xchalu15)
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <float.h>
#include <string.h>


#define SIZE 1024
/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t { //shluk objektu
    int size; //pocet objetku ve shluku - v cluster_t
    int capacity; //kapacita cluster_t (pocet obj_t, pro ktere je rezervovano misto v poli)
    struct obj_t *obj; //ukazatel na pole shluku
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

int check_obj_id(struct cluster_t *arr,int arr_size, int id);

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->size = 0;
    if(cap>0){
        c->obj = malloc(sizeof(struct obj_t)*cap);
        c->capacity = cap;
    }else{
        c->capacity = 0;
        c->obj = NULL;
    }
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    if(c !=NULL){
        free(c->obj);
        c->obj = NULL;
        c->size = 0;
        c->capacity = 0; 
    }
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c != NULL){
        if(c->size == c->capacity){
            c = resize_cluster(c, c->capacity+1);
        }
        if (c->size < c->capacity)
        {
            c->obj[c->size] = obj;
            c->size ++;
        }
    }
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    if(c2->capacity > c1->capacity){
        c1 = resize_cluster(c1,c2->capacity+c1->size);
    }
    
    for (int i = 0; i < c2->size; i++) {
		append_cluster(c1, c2->obj[i]);
	}

    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);
    for (int i = idx; i < narr-1; i++) {
		carr[i] = carr[i + 1];
	}

    return narr - 1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    
    if(o1 != NULL && o2 != NULL){
        float distance = 0.0;
        distance = sqrtf(powf((o2->x) - (o1->x),2.0) + powf((o2->y) - (o1->y),2.0));
        
        return distance;
    }
    
    return 0;

}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float distance = FLT_MAX;
    float distance_new = 0.0;

    for(int j = 0; j<c1->size;j++){
        for(int i = 0; i <c2->size;i++){
            distance_new = obj_distance(&c1->obj[j],&c2->obj[i]);
            if(distance_new < distance){
                distance = distance_new;
            }
        }
    }

    return distance;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float distance = FLT_MAX;
    float distance_new = 0.0;

    for (int i = 0; i < narr; i++) {
		for (int j = i + 1 ; j < narr; j++) {
			distance_new = cluster_distance(carr+i, carr+j);
			if (distance_new < distance) {
				distance = distance_new;
				*c1 = i;
				*c2 = j;
			}
		}
	}
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}
void print_clusters(struct cluster_t *carr, int narr);
/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    *arr = NULL;

    FILE *soubor;
    soubor = fopen(filename, "r");
    char line[SIZE];
    int count_fl = 0; // "count=" hodnota
    int num_of_obj = 0; //pocet skutecne nactenych clusteru

    struct obj_t object;

    if(soubor != NULL){
        fscanf(soubor,"%[^\n]\n",line);
        if(strstr(line, "count=") != NULL){
            sscanf(line, "%*[^-0123456789]%d", &count_fl);

            if(count_fl<0){
                fprintf(stderr, "Chyba v prvnim radku souboru\n");
                return -1;
            }
            
            *arr = malloc(sizeof(struct cluster_t)*count_fl);
            if(*arr == NULL){
                fprintf(stderr,"problem pri alokaci pameti\n");
                return -1;
            }
            
            for(int i = 0; i<count_fl;i++){ //nacitani objektu 
                if((fscanf(soubor,"%[^\n]\n",line)) != EOF){
                    if((sscanf(line,"%d %g %g \n",&object.id, &object.x, &object.y)) == 3){
                        if((object.x >= 0 && object.x <= 1000) && (object.y >= 0 && object.y <= 1000)){  
                            if(check_obj_id(*arr,num_of_obj,object.id)){ //overiju zda jiz nactene id existuje
                                init_cluster(*arr+num_of_obj, 1);
                                append_cluster(*arr+num_of_obj, object);
                                num_of_obj++;
                            }
                        }
                    }    
                }
            }

            while(count_fl != num_of_obj){ //smaze prebytecne clustery, pri situaci kdy je objektu mene nez je dano v "count="  
                count_fl = remove_cluster(*arr,count_fl,count_fl-1);
            }
        }else{
            fprintf(stderr, "Chyba v prvnim radku souboru\n");
            return -1;
        }
    }else{
        fprintf(stderr, "soubor se nepodarilo nacist\n");
        return -1;
    }

    return count_fl;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/*
Overeni objektu se stejnym parametrem ID. 
*/
int check_obj_id(struct cluster_t *arr,int arr_size, int id){
    
    for(int i = 0; i<arr_size;i++){
        if((arr+i)->obj->id == id){
            return 0;
        }
    }
    return 1;
}


int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    int c1 = 0; //pomocna promena pro find_neighbours
    int c2 = 0; //pomocna promena pro find_neighbours
    int obj_count = 0; //pocet nactenych clusteru
    int req_clusters = 1; //pozadovany pocet clusteru, z argumentu
    
    if(argc <= 1 || argc > 4){
        fprintf(stderr, "neplatny argument\n");
        return 1;
    }else if(argc == 3){
        req_clusters = atoi(argv[2]);
    }

    if(req_clusters <= 0){
        fprintf(stderr, "poces vyslednych shluku musi byt > 0 [N>0]\n");
        return 1;
    }

    obj_count = load_clusters(argv[1], &clusters);

    if(obj_count != -1){ //pokud v load_cluster nastala chyba vraci funkce "-1"
        if(req_clusters > obj_count){
            fprintf(stderr, "pozadovany pocet shluku je vyssi nez pocet nactenych shluku\n");
            clear_cluster(clusters);
            clusters = NULL;
            return 1;
        }

        while(obj_count != req_clusters){
            find_neighbours(clusters, obj_count, &c1, &c2);
            merge_clusters(clusters+c1, clusters+c2);
            obj_count = remove_cluster(clusters,obj_count,c2);
        }

        print_clusters(clusters,obj_count);
        clear_cluster(clusters);
        clusters = NULL;
    }

    return 0;
}