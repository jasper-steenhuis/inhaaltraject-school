package com.myexample;

public class BuyStock implements Order {
    private Stock stock;

    public BuyStock(Stock stockToBuy){
        stock = stockToBuy;
    }

    @Override
    public void execute() {
        stock.buy();
    }
}
