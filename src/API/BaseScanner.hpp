#pragma once

#include <boost/dll/shared_library.hpp>
#include <map>
#include <memory>
#include <string>

class BaseScanner : public std::enable_shared_from_this<BaseScanner> {
  public:
    BaseScanner(double PF, double UID);
    virtual ~BaseScanner()                                        = default;
    virtual void UpdateEvent(double Token)                        = 0;
    virtual void StopThat()                                       = 0;
    virtual void ExpressionResult(double key, std::string result) = 0;

    static void UpdateUser(double PF, const std::string& update);

  protected:
    void RegisterScanner(double PF);

  private:
    double PF;
    double UID;
};

using BaseScannerPtrT = std::shared_ptr<BaseScanner>;

using ScannerPointerT = struct ScannerPointerT {
    BaseScannerPtrT            BaseScannerPtr;
    boost::dll::shared_library Library;
};
using ScannerContainerT = std::map<uint64_t, ScannerPointerT>;
