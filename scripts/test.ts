import { AnyAction } from "@wharfkit/session";
import { transact } from "./transact.js";
import { session } from "./config.js";

const action: AnyAction = {
    account: "bridge.eorc",
    authorization: [session.permissionLevel],
    name: "test",
    data: {
        data: "67e6873dc9b7fc061d13f5d537b7940b7f46fca9646174613a2c7b2270223a22656f72633230222c226f70223a227472616e73666572222c227469636b223a22656f7373222c22616d74223a223130227d"
    }
}

await transact([action]);
