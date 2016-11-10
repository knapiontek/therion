
struct family_clazz
{
    int64 person;
    float64 host;
};

family_clazz* family_create()
{
    family_clazz* clazz = malloc(sizeof(family_clazz));
    clazz->person = 123;
    clazz->host = 4.5;
    return clazz;
}

void family_destroy(family_clazz* clazz)
{
    free(clazz);
}

struct main_clazz
{
    family_clazz* clazz;
};

main_clazz* main_create()
{
    main_clazz* clazz = malloc(sizeof(main_clazz));
    clazz->family_clazz = family_create();
    return clazz;
}

void main_destroy(main_clazz* clazz)
{
    family_destroy(clazz->family_clazz);
    free(clazz);
}

void main()
{
    main_clazz* clazz = main_create();
    main_destroy(clazz);
    return 0;
}
