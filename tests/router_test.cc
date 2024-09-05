#include <gtest/gtest.h>
#include <router.h>

TEST(Router, AllowsDeclarations) {
    const auto router_ = router::factory();

    router_->GET("/route", [](request_type &request, response_type &response) {
        return response;
    });

    router_->POST("/route", [](request_type &request, response_type &response) {
        return response;
    });

    router_->PUT("/route", [](request_type &request, response_type &response) {
        return response;
    });

    router_->PATCH("/route", [](request_type &request, response_type &response) {
        return response;
    });

    router_->DELETE("/route", [](request_type &request, response_type &response) {
        return response;
    });
}
