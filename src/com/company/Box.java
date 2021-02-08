package com.company;

import java.util.List;

public class Box
{
    private List<Product> productsList;

    public Box()
    {

    }
    public void add(Product p){
        productsList.add(p);
    }
    public void remove(Product p){
        productsList.remove(p);
    }
}
