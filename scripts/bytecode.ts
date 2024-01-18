import path from "path";
import { Hex } from "viem";

export const filepath = path.join(import.meta.dir, "..", "bin", "BridgeEORC20.bin");
export const bytecode = await Bun.file(filepath).text() as Hex
