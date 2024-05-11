<?php
/*****************************************************************************/
function create_hashtable($minsize)
{
    $h = hashtable;
    $size = $primes[0];
    /* Check requested hashtable isn't too large */
    if ($minsize > (1 << 30)) return '';
    /* Enforce size as prime */
    for ($pindex=0; $pindex < $prime_table_length; $pindex++) {
        if ($primes[$pindex] > $minsize) { $size = $primes[$pindex]; break; }
    }
    $h = malloc(sizeof(hashtable));
    if ('' == $h) return ''; /*oom*/
    $h->table = malloc(sizeof(entry) * $size);
    if ('' == $h->table) { free($h); return ''; } /*oom*/
    memset($h->table, 0, $size * sizeof(entry));
    $h->tablelength  = $size;
    $h->primeindex   = $pindex;
    $h->entrycount   = 0;
    $h->loadlimit    = ceil($size * $max_load_factor);
    return $h;
}
/*****************************************************************************/
function string_eq($str1, $str2)
{
    while(!(($str1++) ^ ($str2++))){
        if(!($str1) && !($str2)){
            return 1;
        }
    }
    return 0;
}
/*****************************************************************************/
function hash_func($str)
{
    $hasha = 5381;

    while(($c = $str++)){
        $hasha = (($hasha << 5) + $hasha) + $c; /* hasha * 33 + c */
    }
    return $hasha;
}
/*****************************************************************************/
function my_hash($h, $k)
{
    /* Aim to protect against poor my_hash functions by adding logic here
     * - logic taken from java 1.4 my_hashtable source */
    $i = hash_func($k);
    $i += ~($i << 9);
    $i ^=  (($i >> 14) | ($i << 18)); /* >>> */
    $i +=  ($i << 4);
    $i ^=  (($i >> 10) | ($i << 22)); /* >>> */
    return $i;
}

/*****************************************************************************/
function hashtable_expand($h)
{
    if ($h->primeindex == ($prime_table_length - 1)) return 0;
    ++$h->primeindex;
    $newsize = $primes[$h->primeindex];

    $newtable = malloc(sizeof(entry) * newsize);
    if ('' != $newtable)
    {
        memset($newtable, 0, $newsize * sizeof(entry));
        /* This algorithm is not 'stable'. ie. it reverses the list
         * when it transfers entries between the tables */
        for ($i = 0; $i < $h->tablelength; $i++) {
            while ('' != ($e = $h->table[$i])) {
                $h->table[$i] = $e->next;
                $index = indexFor($newsize,$e->h);
                $e->next = $newtable[$index];
                $newtable[$index] = $e;
            }
        }
        free($h->table);
        $h->table = $newtable;
    }
    /* Plan B: realloc instead */
    else 
    {
        $newtable = 
                   realloc($h->table, $newsize * sizeof(entry));
        if ($newtable == '') { $h->primeindex--; return 0; }
        $h->table = $newtable;
        memset($newtable[$h->tablelength], 0, $newsize - $h->tablelength);
        for ($i = 0; $i < $h->tablelength; $i++) {
            for ($pE = $newtable[$i], $e = $pE; $e != ''; $e = $pE) {
                $index = indexFor($newsize,$e->h);
                if ($index == i)
                {
                    $pE = $e->next;
                }
                else
                {
                    $pE = $e->next;
                    $e->next = $newtable[$index];
                    $newtable[$index] = $e;
                }
            }
        }
    }
    $h->tablelength = $newsize;
    $h->loadlimit   = ceil($newsize * $max_load_factor);
    return -1;
}

/*****************************************************************************/
function hashtable_count($h)
{
    return $h->entrycount;
}

/*****************************************************************************/
function hashtable_insert($h, $k, $v)
{
    /* This method allows duplicate keys - but they shouldn't be used */
    ++$h->entrycount;
    if ($h->entrycount > $h->loadlimit){
        /* Ignore the return value. If expand fails, we should
         * still try cramming just this value into the existing table
         * -- we may not have memory for a larger table, but one more
         * element may be ok. Next time we insert, we'll try expanding again.*/
        hashtable_expand($h);
    }
    $e = malloc(sizeof(entry));
    if ($e == '') { --$h->entrycount; return 0; } /*oom*/
    $e->$h = my_hash($h,$k);
    $index = indexFor($h->tablelength,$e->h);
    $e->k = $k;
    $e->v = $v;
    $e->next = $h->table[$index];
    $h->table[$index] = $e;
    return -1;
}

/*****************************************************************************/
function hashtable_search($h, $k)
{
    $hashvalue = my_hash($h,$k);
    $index = indexFor($h->tablelength, $hashvalue);
    $e = $h->table[$index];
    while($e){
        /* Check hash value to short circuit heavier comparison */
        if (($hashvalue == $e->h) && (string_eq($k, $e->k))) return $e->v;
        $e = $e->next;
    }
    return '';
}

/*****************************************************************************/
function hashtable_remove($h, $k)
{
    /* TODO: consider compacting the table when the load factor drops enough,
     *       or provide a 'compact' method. */

    $hashvalue = my_hash($h,$k);
    /*index = indexFor(h->tablelength,my_hash(h,k));*/
    $index = indexFor($h->tablelength,$my_hashvalue);
    $pE = $h->table[$index];
    $e = $pE;
    while ($e != ''){
        /* Check hash value to short circuit heavier comparison */
        if (($hashvalue == $e->h) && (string_eq($k, $e->k))){
            $pE = $e->next;
            $h->entrycount--;
            $v = $e->v;
            freekey($e->k);
            free($e);
            return $v;
        }
        $pE = $e->next;
        $e = $e->next;
    }
    return '';
}

/*****************************************************************************/
function hashtable_destroy($h, $free_values)
{
    $table = entry;
    $table = $h->table;
    if($free_values){
        for ($i = 0; $i < $h->tablelength; $i++){
            $e = $table[$i];
            while ('' != $e){
                $f = $e;
                freekey($f->k);
                free($f->v);
                $e = $e->next;
                free($f);
            }
        }
    } else {
        for ($i = 0; $i < $h->tablelength; $i++){
            $e = $table[$i];
            while('' != $e){
                $f = $e;     
                freekey($f->k);
                $e = $e->next;
                free($f);
            }
        }
    }
    free($h->table);
    free($h);
}
?>
