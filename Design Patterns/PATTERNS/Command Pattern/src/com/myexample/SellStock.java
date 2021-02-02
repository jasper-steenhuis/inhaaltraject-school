package com.myexample;

public class SellStock implements Order
{
    private Stock stock;

    public SellStock(Stock stockToSell)
    {
        stock = stockToSell;
    }

    @Override
    public void execute()
    {
        stock.sell();
    }
}
